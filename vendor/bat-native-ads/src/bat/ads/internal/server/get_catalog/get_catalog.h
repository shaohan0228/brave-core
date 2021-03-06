/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_SERVER_GET_CATALOG_GET_CATALOG_H_
#define BAT_ADS_INTERNAL_SERVER_GET_CATALOG_GET_CATALOG_H_

#include <stdint.h>

#include <string>

#include "bat/ads/internal/backoff_timer.h"
#include "bat/ads/internal/timer.h"
#include "bat/ads/mojom.h"
#include "bat/ads/result.h"

namespace ads {

class AdsImpl;

class GetCatalog {
 public:
  GetCatalog(
      AdsImpl* ads);

  ~GetCatalog();

  void MaybeDownload();

  uint64_t LastUpdated() const;

 private:
  void Download();
  void OnDownloaded(
      const UrlResponse& url_response);

  bool Parse(
      const std::string& json);

  void OnSaved(
      const Result result);

  Timer timer_;

  BackoffTimer retry_timer_;
  void Retry();
  void OnRetry();

  void DownloadAfterDelay();

  bool is_processing_ = false;

  uint64_t last_updated_ = 0;

  AdsImpl* ads_;  // NOT OWNED
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_SERVER_GET_CATALOG_GET_CATALOG_H_
