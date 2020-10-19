/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_sync/version_helper.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace brave_sync {

TEST(RemoteSyncVersion, SupportsSelfDelete) {
  // Empty version string
  EXPECT_FALSE(RemoteDeviceSupportsSelfDelete(""));
  // Number of segments less than 3
  EXPECT_FALSE(RemoteDeviceSupportsSelfDelete("1.0.0"));
  // Expected number of segments, but old version
  EXPECT_FALSE(RemoteDeviceSupportsSelfDelete("1.0.0.1"));

  // Full Chromiuim version string, but old version
  EXPECT_FALSE(RemoteDeviceSupportsSelfDelete(
      "86.1.15.72 (Developer Build c69c33933bfc72a159aceb4aeca939eb0087416c-refs/branch-heads/4240@{#1149} Linux) unknown"  // NOLINT
  ));

  // Full Chromiuim version string, but supported version
  EXPECT_TRUE(RemoteDeviceSupportsSelfDelete(
      "86.1.17.28 (Developer Build 7ed88b53bda45a2d19efb4f8706dd6b6cad0d3af-refs/branch-heads/4240@{#1183} Linux)"  // NOLINT
  ));
}

}  // namespace brave_sync
