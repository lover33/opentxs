// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Internal.hpp"

#include "opentxs/api/client/Activity.hpp"
#include "opentxs/api/client/Contacts.hpp"
#include "opentxs/api/client/Manager.hpp"

namespace opentxs::api::client::internal
{
struct Activity : virtual public api::client::Activity {
    virtual void MigrateLegacyThreads() const = 0;
};
struct Contacts : virtual public api::client::Contacts {
    virtual void start() = 0;
};
struct Manager : virtual public api::client::Manager {
    virtual void StartActivity() = 0;
    virtual void StartContacts() = 0;
    virtual opentxs::OTWallet* StartWallet() = 0;
};
}  // namespace opentxs::api::client::internal
