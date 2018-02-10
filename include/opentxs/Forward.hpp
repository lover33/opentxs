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

#ifndef OPENTXS_FORWARD_HPP
#define OPENTXS_FORWARD_HPP

#include "opentxs/Version.hpp"

namespace opentxs
{
namespace api
{
namespace client
{
class Issuer;
class Pair;
class ServerAction;
class Wallet;
}  // namespace opentxs::api::client

namespace crypto
{
class Encode;
class Hash;
class Symmetric;
class Util;
}  // namespace opentxs::api::crypto

namespace storage
{
class Storage;
}  // namespace opentxs::api::storage

namespace network
{
class Dht;
class ZMQ;
}  // namespace opentxs::api::network

class Activity;
class Api;
class Blockchain;
class ContactManager;
class Crypto;
class Identity;
class Native;
class Server;
class Settings;
class Wallet;
}  // namespace opentxs::api

namespace client
{
class ServerAction;
}  // namespace opentxs::client

namespace network
{
namespace zeromq
{
class Context;
class Message;
class ReplySocket;
class RequestSocket;
}  // namespace opentxs::network::zeromq
}  // namespace opentxs::network

class Bip32;
class Bip39;
class Cheque;
class ClientContext;
class Contact;
class Context;
class Credential;
class CryptoAsymmetric;
class CryptoEncoding;
class CryptoHash;
class CryptoSymmetric;
class CryptoSymmetricNew;
class Data;
class Identifier;
class Ledger;
class Log;
class Message;
#if OT_CASH
class Mint;
#endif  // OT_CASH
class Nym;
class NymData;
class OT;
class OT_API;
class OT_ME;
class OTAPI_Exec;
class OTASCIIArmor;
class OTCachedKey;
class OTDataFolder;
class OTFolders;
class OTME_too;
class OTPassword;
class OTPasswordData;
class OTPaths;
class OTPayment;
class OTPaymentPlan;
class OTSmartContract;
class PaymentCode;
class PeerObject;
class Purse;
class ServerConnection;
class ServerContext;
class ServerContract;
class Signals;
class StorageDriver;
class StoragePlugin;
class String;
class SymmetricKey;
class UnitDefinition;
}  // namespace opentxs
#endif  // OPENTXS_FORWARD_HPP
