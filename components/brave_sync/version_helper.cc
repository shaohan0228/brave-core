/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_sync/version_helper.h"

#include <map>
#include <string>
#include <vector>

#include "base/strings/string_split.h"
#include "base/version.h"

namespace {

using MajorToFull = std::map<int, base::Version>;
const MajorToFull can_self_delete_from{
    {17, base::Version({1, 17, 26})},  // Nightly which actually can self delete
    // Below are for test, fill with actual when uplifted
    {16, base::Version({1, 16, 100})},  // Beta & Dev
    {15, base::Version({1, 15, 100})},  // Release

    // For the future
    {18, base::Version({1, 18, 0})},
    {19, base::Version({1, 19, 0})},
    {20, base::Version({1, 20, 0})},
};

}  // namespace

namespace brave_sync {

// Remove this function when all channels will be able to remove itself
bool RemoteDeviceSupportsSelfDelete(const std::string& remote_chrome_string) {
  // Array of versions per each channel which can process delete device request
  // in a proper way
  std::vector<base::StringPiece> segments =
      base::SplitStringPiece(remote_chrome_string, " ", base::TRIM_WHITESPACE,
                             base::SPLIT_WANT_NONEMPTY);
  if (segments.empty()) {
    // Something wrong is in function, assume remove device cannot delete
    // itself properly
    return false;
  }

  base::Version remote_chrome_version(segments[0]);
  if (!remote_chrome_version.IsValid() ||
      remote_chrome_version.components().size() != 4u) {
    return false;
  }

  const auto& rcvc = remote_chrome_version.components();
  base::Version remote_brave_version({rcvc[1], rcvc[2], rcvc[3]});

  MajorToFull::const_iterator it =
      can_self_delete_from.find(remote_brave_version.components()[1]);

  if (it == can_self_delete_from.end()) {
    // The version of remote computer is beyond of anything, so most probably
    // it does not support self delete on remote request
    return false;
  }

  return (remote_brave_version >= it->second);
}

}  // namespace brave_sync
