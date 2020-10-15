/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/common/brave_features.h"
#include "build/build_config.h"

namespace features {

#if defined(OS_ANDROID)
//  Flag for Brave Rewards.
#if defined(ARCH_CPU_X86_FAMILY) && defined(OFFICIAL_BUILD)
const base::Feature kBraveRewards{"BraveRewards",
                                  base::FEATURE_DISABLED_BY_DEFAULT};
#else
const base::Feature kBraveRewards{"BraveRewards",
                                  base::FEATURE_ENABLED_BY_DEFAULT};
#endif
#endif  // defined(OS_ANDROID)

// Toggles Global Privacy Control, which conveys a user's request to websites
// and services to not sell or share their personal information with third
// parties.
const base::Feature kGlobalPrivacyControl{"GlobalPrivacyControl",
                                          base::FEATURE_DISABLED_BY_DEFAULT};

// Controls behaviour of the contextual ad matching mechanism, e.g. by
// adjusting the number of page classifications used to infer user interest.
const base::Feature kContextualAdsControl{
    "ContextualAdsControl", base::FEATURE_DISABLED_BY_DEFAULT};

}  // namespace features
