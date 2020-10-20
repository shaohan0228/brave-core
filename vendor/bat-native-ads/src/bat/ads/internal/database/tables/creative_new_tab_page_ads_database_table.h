/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_DATABASE_CREATIVE_NEW_TAB_PAGE_ADS_DATABASE_TABLE_H_
#define BAT_ADS_INTERNAL_DATABASE_CREATIVE_NEW_TAB_PAGE_ADS_DATABASE_TABLE_H_

#include <memory>
#include <string>
#include <vector>

#include "bat/ads/ads_client.h"
#include "bat/ads/internal/bundle/creative_new_tab_page_ad_info.h"
#include "bat/ads/internal/classification/page_classifier/page_classifier.h"
#include "bat/ads/internal/database/database_table.h"
#include "bat/ads/internal/database/tables/campaigns_database_table.h"
#include "bat/ads/internal/database/tables/categories_database_table.h"
#include "bat/ads/internal/database/tables/creative_ads_database_table.h"
#include "bat/ads/internal/database/tables/dayparts_database_table.h"
#include "bat/ads/internal/database/tables/geo_targets_database_table.h"
#include "bat/ads/mojom.h"
#include "bat/ads/result.h"

namespace ads {

using GetCreativeNewTabPageAdCallback = std::function<void(const Result,
    const std::string& creative_instance_id, const CreativeNewTabPageAdInfo&)>;

using GetCreativeNewTabPageAdsCallback = std::function<void(const Result,
    const std::vector<std::string>&, const CreativeNewTabPageAdList&)>;

class AdsImpl;

namespace database {
namespace table {

class CreativeNewTabPageAds : public Table {
 public:
  explicit CreativeNewTabPageAds(
      AdsImpl* ads);

  ~CreativeNewTabPageAds() override;

  void Save(
      const CreativeNewTabPageAdList& creative_new_tab_page_ads,
      ResultCallback callback);

  void Delete(
      ResultCallback callback);

  void GetForCreativeInstanceId(
      const std::string& creative_instance_id,
      GetCreativeNewTabPageAdCallback callback);

  void GetForCategories(
      const classification::CategoryList& categories,
      GetCreativeNewTabPageAdsCallback callback);

  void GetAll(
      GetCreativeNewTabPageAdsCallback callback);

  void set_batch_size(
      const int batch_size);

  std::string get_table_name() const override;

  void Migrate(
      DBTransaction* transaction,
      const int to_version) override;

 private:
  void InsertOrUpdate(
      DBTransaction* transaction,
      const CreativeNewTabPageAdList& creative_new_tab_page_ads);

  int BindParameters(
      DBCommand* command,
      const CreativeNewTabPageAdList& creative_new_tab_page_ads);

  std::string BuildInsertOrUpdateQuery(
      DBCommand* command,
      const CreativeNewTabPageAdList& creative_new_tab_page_ads);

  void OnGetForCreativeInstanceId(
      DBCommandResponsePtr response,
      const std::string& creative_instance_id,
      GetCreativeNewTabPageAdCallback callback);

  void OnGetForCategories(
      DBCommandResponsePtr response,
      const classification::CategoryList& categories,
      GetCreativeNewTabPageAdsCallback callback);

  void OnGetAll(
      DBCommandResponsePtr response,
      GetCreativeNewTabPageAdsCallback callback);

  CreativeNewTabPageAdInfo GetFromRecord(
      DBRecord* record) const;

  void CreateTableV3(
      DBTransaction* transaction);
  void MigrateToV3(
      DBTransaction* transaction);

  int batch_size_;

  AdsImpl* ads_;  // NOT OWNED

  std::unique_ptr<Campaigns> campaigns_database_table_;
  std::unique_ptr<Categories> categories_database_table_;
  std::unique_ptr<CreativeAds> creative_ads_database_table_;
  std::unique_ptr<Dayparts> dayparts_database_table_;
  std::unique_ptr<GeoTargets> geo_targets_database_table_;
};

}  // namespace table
}  // namespace database
}  // namespace ads

#endif  // BAT_ADS_INTERNAL_DATABASE_CREATIVE_NEW_TAB_PAGE_ADS_DATABASE_TABLE_H_
