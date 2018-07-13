/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include "stdafx.hpp"

#include "opentxs/api/crypto/Crypto.hpp"
#include "opentxs/api/crypto/Hash.hpp"
#include "opentxs/api/crypto/Symmetric.hpp"
#include "opentxs/api/crypto/Util.hpp"
#include "opentxs/api/Native.hpp"
#include "opentxs/core/crypto/Crypto.hpp"
#include "opentxs/core/crypto/OTPassword.hpp"
#include "opentxs/core/crypto/OTPasswordData.hpp"
#include "opentxs/core/Data.hpp"
#include "opentxs/core/Log.hpp"
#include "opentxs/crypto/key/EllipticCurve.hpp"
#include "opentxs/crypto/key/Symmetric.hpp"
#include "opentxs/crypto/library/HashingProvider.hpp"
#include "opentxs/crypto/library/LegacySymmetricProvider.hpp"
#include "opentxs/OT.hpp"

#include "EcdsaProvider.hpp"

namespace opentxs::crypto
{
bool EcdsaProvider::DecryptPrivateKey(
    const proto::Ciphertext& encryptedKey,
    const OTPasswordData& password,
    OTPassword& plaintextKey)
{
    auto key = OT::App().Crypto().Symmetric().Key(
        encryptedKey.key(), encryptedKey.mode());

    if (!key.get()) { return false; }

    return key->Decrypt(encryptedKey, password, plaintextKey);
}

bool EcdsaProvider::DecryptPrivateKey(
    const proto::Ciphertext& encryptedKey,
    const proto::Ciphertext& encryptedChaincode,
    const OTPasswordData& password,
    OTPassword& key,
    OTPassword& chaincode)
{
    auto sessionKey = OT::App().Crypto().Symmetric().Key(
        encryptedKey.key(), encryptedKey.mode());

    const bool keyDecrypted = sessionKey->Decrypt(encryptedKey, password, key);
    const bool chaincodeDecrypted =
        sessionKey->Decrypt(encryptedChaincode, password, chaincode);

    return (keyDecrypted && chaincodeDecrypted);
}

bool EcdsaProvider::EncryptPrivateKey(
    const OTPassword& plaintextKey,
    const OTPasswordData& password,
    proto::Ciphertext& encryptedKey)
{
    auto key = OT::App().Crypto().Symmetric().Key(password);

    if (!key.get()) { return false; }

    auto blank = Data::Factory();

    return key->Encrypt(plaintextKey, blank, password, encryptedKey, true);
}

bool EcdsaProvider::EncryptPrivateKey(
    const OTPassword& key,
    const OTPassword& chaincode,
    const OTPasswordData& password,
    proto::Ciphertext& encryptedKey,
    proto::Ciphertext& encryptedChaincode)
{
    auto sessionKey = OT::App().Crypto().Symmetric().Key(password);

    if (!sessionKey.get()) { return false; }

    auto blank = Data::Factory();
    const bool keyEncrypted =
        sessionKey->Encrypt(key, blank, password, encryptedKey, true);
    const bool chaincodeEncrypted = sessionKey->Encrypt(
        chaincode, blank, password, encryptedChaincode, false);

    return (keyEncrypted && chaincodeEncrypted);
}
}  // namespace opentxs::crypto

namespace opentxs::crypto::implementation
{
bool EcdsaProvider::AsymmetricKeyToECPrivatekey(
    const crypto::key::EllipticCurve& asymmetricKey,
    const OTPasswordData& passwordData,
    OTPassword& privkey) const
{
    proto::Ciphertext dataPrivkey;
    const bool havePrivateKey = asymmetricKey.GetKey(dataPrivkey);

    if (!havePrivateKey) { return false; }

    return AsymmetricKeyToECPrivkey(dataPrivkey, passwordData, privkey);
}

bool EcdsaProvider::AsymmetricKeyToECPrivkey(
    const proto::Ciphertext& asymmetricKey,
    const OTPasswordData& passwordData,
    OTPassword& privkey) const
{
    BinarySecret masterPassword(
        OT::App().Crypto().AES().InstantiateBinarySecretSP());

    return ImportECPrivatekey(asymmetricKey, passwordData, privkey);
}

bool EcdsaProvider::AsymmetricKeyToECPubkey(
    const crypto::key::EllipticCurve& asymmetricKey,
    Data& pubkey) const
{
    return asymmetricKey.GetKey(pubkey);
}

bool EcdsaProvider::DecryptSessionKeyECDH(
    const crypto::key::EllipticCurve& privateKey,
    const crypto::key::EllipticCurve& publicKey,
    const OTPasswordData& password,
    crypto::key::Symmetric& sessionKey) const
{
    auto publicDHKey = Data::Factory();

    if (!publicKey.GetKey(publicDHKey)) {
        otErr << __FUNCTION__ << ": Failed to get public key." << std::endl;

        return false;
    }

    OTPassword privateDHKey;

    if (!AsymmetricKeyToECPrivatekey(privateKey, password, privateDHKey)) {
        otErr << __FUNCTION__ << ": Failed to get private key." << std::endl;

        return false;
    }

    // Calculate ECDH shared secret
    BinarySecret ECDHSecret(
        OT::App().Crypto().AES().InstantiateBinarySecretSP());
    bool haveECDH = ECDH(publicDHKey, privateDHKey, *ECDHSecret);

    if (!haveECDH) {
        otErr << __FUNCTION__ << ": ECDH shared secret negotiation failed."
              << std::endl;

        return false;
    }

    OTPasswordData unlockPassword("");
    unlockPassword.SetOverride(*ECDHSecret);

    return sessionKey.Unlock(unlockPassword);
}

bool EcdsaProvider::ECPrivatekeyToAsymmetricKey(
    const OTPassword& privkey,
    const OTPasswordData& passwordData,
    crypto::key::EllipticCurve& asymmetricKey) const
{
    return ExportECPrivatekey(privkey, passwordData, asymmetricKey);
}

bool EcdsaProvider::ECPubkeyToAsymmetricKey(
    const Data& pubkey,
    crypto::key::EllipticCurve& asymmetricKey) const
{
    if (pubkey.empty()) { return false; }

    return asymmetricKey.SetKey(pubkey);
}

bool EcdsaProvider::EncryptSessionKeyECDH(
    const crypto::key::EllipticCurve& privateKey,
    const crypto::key::EllipticCurve& publicKey,
    const OTPasswordData& passwordData,
    crypto::key::Symmetric& sessionKey,
    OTPassword& newKeyPassword) const
{
    auto dhPublicKey = Data::Factory();
    const auto havePubkey = publicKey.GetKey(dhPublicKey);

    if (!havePubkey) {
        otErr << __FUNCTION__ << ": Failed to get public key." << std::endl;

        return false;
    }

    BinarySecret dhPrivateKey(
        OT::App().Crypto().AES().InstantiateBinarySecretSP());

    OT_ASSERT(dhPrivateKey);

    OTPasswordData privatePassword("");
    const bool havePrivateKey =
        AsymmetricKeyToECPrivatekey(privateKey, privatePassword, *dhPrivateKey);

    if (!havePrivateKey) {
        otErr << __FUNCTION__ << ": Failed to get private key." << std::endl;

        return false;
    }

    // Calculate ECDH shared secret
    const bool haveECDH = ECDH(dhPublicKey, *dhPrivateKey, newKeyPassword);

    if (!haveECDH) {
        otErr << __FUNCTION__ << ": ECDH shared secret negotiation failed."
              << std::endl;

        return false;
    }

    const bool encrypted =
        sessionKey.ChangePassword(passwordData, newKeyPassword);

    if (!encrypted) {
        otErr << __FUNCTION__ << ": Session key encryption failed."
              << std::endl;

        return false;
    }

    return true;
}

bool EcdsaProvider::ExportECPrivatekey(
    const OTPassword& privkey,
    const OTPasswordData& password,
    crypto::key::EllipticCurve& asymmetricKey) const
{
    std::unique_ptr<proto::Ciphertext> encryptedKey(new proto::Ciphertext);

    EncryptPrivateKey(privkey, password, *encryptedKey);

    return asymmetricKey.SetKey(encryptedKey);
}

bool EcdsaProvider::ImportECPrivatekey(
    const proto::Ciphertext& asymmetricKey,
    const OTPasswordData& password,
    OTPassword& privkey) const
{
    return DecryptPrivateKey(asymmetricKey, password, privkey);
}

bool EcdsaProvider::PrivateToPublic(
    const proto::AsymmetricKey& privateKey,
    proto::AsymmetricKey& publicKey) const
{
    publicKey.CopyFrom(privateKey);
    publicKey.clear_chaincode();
    publicKey.clear_key();
    publicKey.set_mode(proto::KEYMODE_PUBLIC);
    auto key = Data::Factory();

    if (false == PrivateToPublic(privateKey.encryptedkey(), key)) {

        return false;
    }

    publicKey.set_key(key->GetPointer(), key->GetSize());

    return true;
}

bool EcdsaProvider::PrivateToPublic(
    const proto::Ciphertext& privateKey,
    Data& publicKey) const
{
    BinarySecret plaintextKey(
        OT::App().Crypto().AES().InstantiateBinarySecretSP());
    OTPasswordData password(__FUNCTION__);
    const bool decrypted =
        DecryptPrivateKey(privateKey, password, *plaintextKey);

    if (!decrypted) { return false; }

    return ScalarBaseMultiply(*plaintextKey, publicKey);
}

bool EcdsaProvider::SeedToCurveKey(
    [[maybe_unused]] const OTPassword& seed,
    [[maybe_unused]] OTPassword& privateKey,
    [[maybe_unused]] Data& publicKey) const
{
    otErr << __FUNCTION__ << ": this provider does not support curve25519."
          << std::endl;

    return false;
}
}  // namespace opentxs::crypto::implementation
