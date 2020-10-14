/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SYNC_VERSION_HELPER_H_
#define BRAVE_COMPONENTS_BRAVE_SYNC_VERSION_HELPER_H_

#include <string>

namespace brave_sync {

bool RemoteDeviceSupportsSelfDelete(const std::string& remote_chrome_string);

}  // namespace brave_sync

#endif  // BRAVE_COMPONENTS_BRAVE_SYNC_VERSION_HELPER_H_
