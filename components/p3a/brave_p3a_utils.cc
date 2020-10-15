/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/p3a/brave_p3a_utils.h"

#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "brave/components/p3a/pref_names.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace brave_shields {

void MaybeRecordShieldsUsageP3A(ShieldsIconUsage usage,
                                PrefService* local_state) {
  // May be null in tests.
  if (!local_state) {
    return;
  }
  int last_value = local_state->GetInteger(brave::kShieldUsageStatus);
  if (last_value < usage) {
    UMA_HISTOGRAM_ENUMERATION("Brave.Shields.UsageStatus", usage, kSize);
    local_state->SetInteger(brave::kShieldUsageStatus, usage);
  }
}

}  // namespace brave_shields

namespace brave {

void MaybeRecordNTPCustomizeUsageP3A(NTPCustomizeUsage usage,
                                     PrefService* local_state) {
  // May be null in tests.
  if (!local_state) {
    return;
  }
  int last_value = local_state->GetInteger(kNTPCustomizeUsageStatus);
  if (last_value < usage) {
    UMA_HISTOGRAM_ENUMERATION("Brave.NTP.CustomizeUsageStatus", usage,
        kCustomizeUsageMax);
    local_state->SetInteger(kNTPCustomizeUsageStatus, usage);
  }
}

std::vector<std::string> prefs_to_register_;

void RegisterP3ARecordedUsageInt(PrefRegistrySimple* local_state) {
  // TODO: ..
  for (auto pref : prefs_to_register_) {
    LOG(ERROR) << "BSC]] demo] " << pref;
    //local_state->RegisterIntegerPref(kShieldUsageStatus, -1);
  }
}

template <typename T> class P3ARecordedUsageInt {
 public:
  P3ARecordedUsageInt(T max_value, std::string pref_name,
      std::string event_name)
      : enum_max_value_(max_value),
        pref_name_(pref_name),
        event_name_(event_name) {
    DCHECK(std::is_enum<T>::value);
    prefs_to_register_.push_back(pref_name_);
  }

  void MaybeRecordUsageIntP3A(T new_value, PrefService* local_state) {
    // May be null in tests.
    if (!local_state) {
      return;
    }
    int last_value = local_state->GetInteger(pref_name_.c_str());
    int new_value_int = static_cast<int>(new_value);
    if (last_value < new_value_int) {
      // UMA_HISTOGRAM_ENUMERATION(event_name.c_str(), new_value,
      //     params.enum_max_value);
      LOG(ERROR) << "BSC]]" << (int)enum_max_value_;
      local_state->SetInteger(pref_name_.c_str(), new_value_int);
    }
  }

 private:
  T enum_max_value_;
  std::string pref_name_;
  std::string event_name_;
};

void RegisterP3AUtilsPrefs(PrefRegistrySimple* local_state) {
  // example usage (this is what would go into an implementation)
  P3ARecordedUsageInt<NTPCustomizeUsage> usage(
      kCustomizeUsageMax,
      kNTPCustomizeUsageStatus,
      "Brave.NTP.CustomizeUsageStatus");
  usage.MaybeRecordUsageIntP3A(kNeverOpened, NULL);

  //...
  RegisterP3ARecordedUsageInt(local_state);


  //old code
  local_state->RegisterIntegerPref(kShieldUsageStatus, -1);
  local_state->RegisterIntegerPref(kNTPCustomizeUsageStatus, -1);
}

}  // namespace brave
