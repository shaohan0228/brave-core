/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_DATABASE_DAYPARTS_DATABASE_TABLE_H_
#define BAT_ADS_INTERNAL_DATABASE_DAYPARTS_DATABASE_TABLE_H_

#include <string>

#include "bat/ads/ads_client.h"
#include "bat/ads/internal/bundle/creative_ad_info.h"
#include "bat/ads/internal/database/database_table.h"

namespace ads {

class AdsImpl;

namespace database {
namespace table {

class Dayparts : public Table {
 public:
  explicit Dayparts(
      AdsImpl* ads);

  ~Dayparts() override;

  void InsertOrUpdate(
      DBTransaction* transaction,
      const CreativeAdList& creative_ads);

  void Delete(
      ResultCallback callback);

  std::string get_table_name() const override;

  void Migrate(
      DBTransaction* transaction,
      const int to_version) override;

 private:
  int BindParameters(
      DBCommand* command,
      const CreativeAdList& creative_ads);

  std::string BuildInsertOrUpdateQuery(
      DBCommand* command,
      const CreativeAdList& creative_ads);

  void CreateTableV4(
      DBTransaction* transaction);
  void CreateIndexV4(
      DBTransaction* transaction);
  void MigrateToV4(
      DBTransaction* transaction);

  AdsImpl* ads_;  // NOT OWNED
};

}  // namespace table
}  // namespace database
}  // namespace ads

#endif  // BAT_ADS_INTERNAL_DATABASE_DAYPARTS_DATABASE_TABLE_H_
