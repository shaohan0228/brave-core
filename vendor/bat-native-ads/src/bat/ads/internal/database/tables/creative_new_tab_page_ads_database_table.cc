/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/database/tables/creative_new_tab_page_ads_database_table.h"

#include <functional>
#include <set>
#include <utility>

#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/container_util.h"
#include "bat/ads/internal/database/database_statement_util.h"
#include "bat/ads/internal/database/database_table_util.h"
#include "bat/ads/internal/database/database_util.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/time_util.h"

namespace ads {
namespace database {
namespace table {

using std::placeholders::_1;

namespace {

const char kTableName[] = "creative_new_tab_page_ads";

const int kDefaultBatchSize = 50;

}  // namespace

CreativeNewTabPageAds::CreativeNewTabPageAds(
    AdsImpl* ads)
    : batch_size_(kDefaultBatchSize),
      ads_(ads),
      campaigns_database_table_(std::make_unique<Campaigns>(ads_)),
      categories_database_table_(std::make_unique<Categories>(ads_)),
      creative_ads_database_table_(std::make_unique<CreativeAds>(ads_)),
      dayparts_database_table_(std::make_unique<Dayparts>(ads_)),
      geo_targets_database_table_(std::make_unique<GeoTargets>(ads_)) {
  DCHECK(ads_);
}

CreativeNewTabPageAds::~CreativeNewTabPageAds() = default;

void CreativeNewTabPageAds::Save(
    const CreativeNewTabPageAdList& creative_new_tab_page_ads,
    ResultCallback callback) {
  if (creative_new_tab_page_ads.empty()) {
    callback(Result::SUCCESS);
    return;
  }

  DBTransactionPtr transaction = DBTransaction::New();

  const std::vector<CreativeNewTabPageAdList> batches =
      SplitVector(creative_new_tab_page_ads, batch_size_);

  for (const auto& batch : batches) {
    InsertOrUpdate(transaction.get(), batch);

    std::vector<CreativeAdInfo> creative_ads(batch.begin(), batch.end());
    campaigns_database_table_->InsertOrUpdate(
        transaction.get(), creative_ads);
    categories_database_table_->InsertOrUpdate(
        transaction.get(), creative_ads);
    creative_ads_database_table_->InsertOrUpdate(
        transaction.get(), creative_ads);
    dayparts_database_table_->InsertOrUpdate(transaction.get(), creative_ads);
    geo_targets_database_table_->InsertOrUpdate(
        transaction.get(), creative_ads);
  }

  ads_->get_ads_client()->RunDBTransaction(std::move(transaction),
      std::bind(&OnResultCallback, _1, callback));
}

void CreativeNewTabPageAds::Delete(
    ResultCallback callback) {
  DBTransactionPtr transaction = DBTransaction::New();

  util::Delete(transaction.get(), get_table_name());

  ads_->get_ads_client()->RunDBTransaction(std::move(transaction),
      std::bind(&OnResultCallback, _1, callback));
}

void CreativeNewTabPageAds::GetForCreativeInstanceId(
    const std::string& creative_instance_id,
    GetCreativeNewTabPageAdCallback callback) {
  CreativeNewTabPageAdInfo creative_new_tab_page_ad;

  if (creative_instance_id.empty()) {
    callback(Result::FAILED, creative_instance_id, creative_new_tab_page_ad);
    return;
  }

  const std::string query = base::StringPrintf(
      "SELECT "
          "can.creative_instance_id, "
          "can.creative_set_id, "
          "can.campaign_id, "
          "cam.start_at_timestamp, "
          "cam.end_at_timestamp, "
          "cam.daily_cap, "
          "cam.advertiser_id, "
          "cam.priority, "
          "ca.conversion, "
          "ca.per_day, "
          "ca.total_max, "
          "c.category, "
          "gt.geo_target, "
          "ca.target_url, "
          "can.company_name, "
          "can.alt, "
          "cam.ptr, "
          "dp.dow, "
          "dp.start_minute, "
          "dp.end_minute "
      "FROM %s AS can "
          "INNER JOIN campaigns AS cam "
              "ON cam.campaign_id = can.campaign_id "
          "INNER JOIN categories AS c "
              "ON c.creative_set_id = can.creative_set_id "
          "INNER JOIN creative_ads AS ca "
              "ON ca.creative_set_id = can.creative_set_id "
          "INNER JOIN geo_targets AS gt "
              "ON gt.campaign_id = can.campaign_id "
          "INNER JOIN dayparts AS dp "
              "ON dp.campaign_id = can.campaign_id "
      "WHERE can.creative_instance_id = '%s'",
      get_table_name().c_str(),
      creative_instance_id.c_str());

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::READ;
  command->command = query;

  command->record_bindings = {
    DBCommand::RecordBindingType::STRING_TYPE,  // creative_instance_id
    DBCommand::RecordBindingType::STRING_TYPE,  // creative_set_id
    DBCommand::RecordBindingType::STRING_TYPE,  // campaign_id
    DBCommand::RecordBindingType::INT64_TYPE,   // start_at_timestamp
    DBCommand::RecordBindingType::INT64_TYPE,   // end_at_timestamp
    DBCommand::RecordBindingType::INT_TYPE,     // daily_cap
    DBCommand::RecordBindingType::STRING_TYPE,  // advertiser_id
    DBCommand::RecordBindingType::INT_TYPE,     // priority
    DBCommand::RecordBindingType::BOOL_TYPE,    // conversion
    DBCommand::RecordBindingType::INT_TYPE,     // per_day
    DBCommand::RecordBindingType::INT_TYPE,     // total_max
    DBCommand::RecordBindingType::STRING_TYPE,  // category
    DBCommand::RecordBindingType::STRING_TYPE,  // geo_target
    DBCommand::RecordBindingType::STRING_TYPE,  // target_url
    DBCommand::RecordBindingType::STRING_TYPE,  // company_name
    DBCommand::RecordBindingType::STRING_TYPE,  // alt
    DBCommand::RecordBindingType::DOUBLE_TYPE,  // ptr
    DBCommand::RecordBindingType::STRING_TYPE,  // dayparts->dow
    DBCommand::RecordBindingType::INT_TYPE,     // dayparts->start_minute
    DBCommand::RecordBindingType::INT_TYPE      // dayparts->end_minute
  };

  DBTransactionPtr transaction = DBTransaction::New();
  transaction->commands.push_back(std::move(command));

  ads_->get_ads_client()->RunDBTransaction(std::move(transaction),
      std::bind(&CreativeNewTabPageAds::OnGetForCreativeInstanceId, this, _1,
          creative_instance_id, callback));
}

void CreativeNewTabPageAds::GetForCategories(
    const classification::CategoryList& categories,
    GetCreativeNewTabPageAdsCallback callback) {
  if (categories.empty()) {
    callback(Result::SUCCESS, categories, {});
    return;
  }

  const std::string query = base::StringPrintf(
      "SELECT "
          "can.creative_instance_id, "
          "can.creative_set_id, "
          "can.campaign_id, "
          "cam.start_at_timestamp, "
          "cam.end_at_timestamp, "
          "cam.daily_cap, "
          "cam.advertiser_id, "
          "cam.priority, "
          "ca.conversion, "
          "ca.per_day, "
          "ca.total_max, "
          "c.category, "
          "gt.geo_target, "
          "ca.target_url, "
          "can.company_name, "
          "can.alt, "
          "cam.ptr, "
          "dp.dow, "
          "dp.start_minute, "
          "dp.end_minute "
      "FROM %s AS can "
          "INNER JOIN campaigns AS cam "
              "ON cam.campaign_id = can.campaign_id "
          "INNER JOIN categories AS c "
              "ON c.creative_set_id = can.creative_set_id "
          "INNER JOIN creative_ads AS ca "
              "ON ca.creative_set_id = can.creative_set_id "
          "INNER JOIN geo_targets AS gt "
              "ON gt.campaign_id = can.campaign_id "
          "INNER JOIN dayparts AS dp "
              "ON dp.campaign_id = can.campaign_id "
      "WHERE c.category IN %s "
          "AND %s BETWEEN cam.start_at_timestamp AND cam.end_at_timestamp",
      get_table_name().c_str(),
      BuildBindingParameterPlaceholder(categories.size()).c_str(),
      NowAsString().c_str());

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::READ;
  command->command = query;

  int index = 0;
  for (const auto& category : categories) {
    BindString(command.get(), index, base::ToLowerASCII(category));
    index++;
  }

  command->record_bindings = {
    DBCommand::RecordBindingType::STRING_TYPE,  // creative_instance_id
    DBCommand::RecordBindingType::STRING_TYPE,  // creative_set_id
    DBCommand::RecordBindingType::STRING_TYPE,  // campaign_id
    DBCommand::RecordBindingType::INT64_TYPE,   // start_at_timestamp
    DBCommand::RecordBindingType::INT64_TYPE,   // end_at_timestamp
    DBCommand::RecordBindingType::INT_TYPE,     // daily_cap
    DBCommand::RecordBindingType::STRING_TYPE,  // advertiser_id
    DBCommand::RecordBindingType::INT_TYPE,     // priority
    DBCommand::RecordBindingType::BOOL_TYPE,    // conversion
    DBCommand::RecordBindingType::INT_TYPE,     // per_day
    DBCommand::RecordBindingType::INT_TYPE,     // total_max
    DBCommand::RecordBindingType::STRING_TYPE,  // category
    DBCommand::RecordBindingType::STRING_TYPE,  // geo_target
    DBCommand::RecordBindingType::STRING_TYPE,  // target_url
    DBCommand::RecordBindingType::STRING_TYPE,  // company_name
    DBCommand::RecordBindingType::STRING_TYPE,  // alt
    DBCommand::RecordBindingType::DOUBLE_TYPE,  // ptr
    DBCommand::RecordBindingType::STRING_TYPE,  // dayparts->dow
    DBCommand::RecordBindingType::INT_TYPE,     // dayparts->start_minute
    DBCommand::RecordBindingType::INT_TYPE      // dayparts->end_minute
  };

  DBTransactionPtr transaction = DBTransaction::New();
  transaction->commands.push_back(std::move(command));

  ads_->get_ads_client()->RunDBTransaction(std::move(transaction),
      std::bind(&CreativeNewTabPageAds::OnGetForCategories, this, _1,
          categories, callback));
}

void CreativeNewTabPageAds::GetAll(
    GetCreativeNewTabPageAdsCallback callback) {
  const std::string query = base::StringPrintf(
      "SELECT "
          "can.creative_instance_id, "
          "can.creative_set_id, "
          "can.campaign_id, "
          "cam.start_at_timestamp, "
          "cam.end_at_timestamp, "
          "cam.daily_cap, "
          "cam.advertiser_id, "
          "cam.priority, "
          "ca.conversion, "
          "ca.per_day, "
          "ca.total_max, "
          "c.category, "
          "gt.geo_target, "
          "ca.target_url, "
          "can.company_name, "
          "can.alt, "
          "cam.ptr, "
          "dp.dow, "
          "dp.start_minute, "
          "dp.end_minute "
      "FROM %s AS can "
          "INNER JOIN campaigns AS cam "
              "ON cam.campaign_id = can.campaign_id "
          "INNER JOIN categories AS c "
              "ON c.creative_set_id = can.creative_set_id "
          "INNER JOIN creative_ads AS ca "
              "ON ca.creative_set_id = can.creative_set_id "
          "INNER JOIN geo_targets AS gt "
              "ON gt.campaign_id = can.campaign_id "
          "INNER JOIN dayparts AS dp "
              "ON dp.campaign_id = can.campaign_id "
      "WHERE %s BETWEEN cam.start_at_timestamp AND cam.end_at_timestamp",
      get_table_name().c_str(),
      NowAsString().c_str());

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::READ;
  command->command = query;

  command->record_bindings = {
    DBCommand::RecordBindingType::STRING_TYPE,  // creative_instance_id
    DBCommand::RecordBindingType::STRING_TYPE,  // creative_set_id
    DBCommand::RecordBindingType::STRING_TYPE,  // campaign_id
    DBCommand::RecordBindingType::INT64_TYPE,   // start_at_timestamp
    DBCommand::RecordBindingType::INT64_TYPE,   // end_at_timestamp
    DBCommand::RecordBindingType::INT_TYPE,     // daily_cap
    DBCommand::RecordBindingType::STRING_TYPE,  // advertiser_id
    DBCommand::RecordBindingType::INT_TYPE,     // priority
    DBCommand::RecordBindingType::BOOL_TYPE,    // conversion
    DBCommand::RecordBindingType::INT_TYPE,     // per_day
    DBCommand::RecordBindingType::INT_TYPE,     // total_max
    DBCommand::RecordBindingType::STRING_TYPE,  // category
    DBCommand::RecordBindingType::STRING_TYPE,  // geo_target
    DBCommand::RecordBindingType::STRING_TYPE,  // target_url
    DBCommand::RecordBindingType::STRING_TYPE,  // company_name
    DBCommand::RecordBindingType::STRING_TYPE,  // alt
    DBCommand::RecordBindingType::DOUBLE_TYPE,  // ptr
    DBCommand::RecordBindingType::STRING_TYPE,  // dayparts->dow
    DBCommand::RecordBindingType::INT_TYPE,     // dayparts->start_minute
    DBCommand::RecordBindingType::INT_TYPE      // dayparts->end_minute
  };

  DBTransactionPtr transaction = DBTransaction::New();
  transaction->commands.push_back(std::move(command));

  ads_->get_ads_client()->RunDBTransaction(std::move(transaction),
      std::bind(&CreativeNewTabPageAds::OnGetAll, this, _1, callback));
}

void CreativeNewTabPageAds::set_batch_size(
    const int batch_size) {
  DCHECK_GT(batch_size, 0);

  batch_size_ = batch_size;
}

std::string CreativeNewTabPageAds::get_table_name() const {
  return kTableName;
}

void CreativeNewTabPageAds::Migrate(
    DBTransaction* transaction,
    const int to_version) {
  DCHECK(transaction);

  switch (to_version) {
    case 3: {
      MigrateToV3(transaction);
      break;
    }

    default: {
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

void CreativeNewTabPageAds::InsertOrUpdate(
    DBTransaction* transaction,
    const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
  DCHECK(transaction);

  if (creative_new_tab_page_ads.empty()) {
    return;
  }

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::RUN;
  command->command = BuildInsertOrUpdateQuery(command.get(),
      creative_new_tab_page_ads);

  transaction->commands.push_back(std::move(command));
}

int CreativeNewTabPageAds::BindParameters(
    DBCommand* command,
    const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
  DCHECK(command);

  int count = 0;

  int index = 0;
  for (const auto& creative_new_tab_page_ad : creative_new_tab_page_ads) {
    BindString(command, index++, creative_new_tab_page_ad.creative_instance_id);
    BindString(command, index++, creative_new_tab_page_ad.creative_set_id);
    BindString(command, index++, creative_new_tab_page_ad.campaign_id);
    BindString(command, index++, creative_new_tab_page_ad.company_name);
    BindString(command, index++, creative_new_tab_page_ad.alt);

    count++;
  }

  return count;
}

std::string CreativeNewTabPageAds::BuildInsertOrUpdateQuery(
    DBCommand* command,
    const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
  const int count = BindParameters(command, creative_new_tab_page_ads);

  return base::StringPrintf(
      "INSERT OR REPLACE INTO %s "
          "(creative_instance_id, "
          "creative_set_id, "
          "campaign_id, "
          "company_name, "
          "alt) VALUES %s",
      get_table_name().c_str(),
      BuildBindingParameterPlaceholders(5, count).c_str());
}

void CreativeNewTabPageAds::OnGetForCreativeInstanceId(
    DBCommandResponsePtr response,
    const std::string& creative_instance_id,
    GetCreativeNewTabPageAdCallback callback) {
  if (!response || response->status != DBCommandResponse::Status::RESPONSE_OK) {
    BLOG(0, "Failed to get creative new tab page ad");
    callback(Result::FAILED, creative_instance_id, {});
    return;
  }

  if (response->result->get_records().size() != 1) {
    BLOG(0, "Failed to get creative new tab page ad");
    callback(Result::FAILED, creative_instance_id, {});
    return;
  }

  DBRecord* record = response->result->get_records().at(0).get();

  const CreativeNewTabPageAdInfo creative_new_tab_page_ad =
      GetFromRecord(record);

  callback(Result::SUCCESS, creative_instance_id, creative_new_tab_page_ad);
}

void CreativeNewTabPageAds::OnGetForCategories(
    DBCommandResponsePtr response,
    const classification::CategoryList& categories,
    GetCreativeNewTabPageAdsCallback callback) {
  if (!response || response->status != DBCommandResponse::Status::RESPONSE_OK) {
    BLOG(0, "Failed to get creative new tab page ads");
    callback(Result::FAILED, categories, {});
    return;
  }

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  for (const auto& record : response->result->get_records()) {
    const CreativeNewTabPageAdInfo creative_new_tab_page_ad =
        GetFromRecord(record.get());

    creative_new_tab_page_ads.emplace_back(creative_new_tab_page_ad);
  }

  callback(Result::SUCCESS, categories, creative_new_tab_page_ads);
}

void CreativeNewTabPageAds::OnGetAll(
    DBCommandResponsePtr response,
    GetCreativeNewTabPageAdsCallback callback) {
  if (!response || response->status != DBCommandResponse::Status::RESPONSE_OK) {
    BLOG(0, "Failed to get all creative new tab page ads");
    callback(Result::FAILED, {}, {});
    return;
  }

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  std::set<std::string> categories;

  for (const auto& record : response->result->get_records()) {
    const CreativeNewTabPageAdInfo creative_new_tab_page_ad =
        GetFromRecord(record.get());

    creative_new_tab_page_ads.emplace_back(creative_new_tab_page_ad);

    categories.insert(creative_new_tab_page_ad.category);
  }

  classification::CategoryList normalized_categories;
  for (const auto& category : categories) {
    normalized_categories.push_back(category);
  }

  callback(Result::SUCCESS, normalized_categories, creative_new_tab_page_ads);
}

CreativeNewTabPageAdInfo CreativeNewTabPageAds::GetFromRecord(
    DBRecord* record) const {
  CreativeNewTabPageAdInfo creative_new_tab_page_ad;

  creative_new_tab_page_ad.creative_instance_id = ColumnString(record, 0);
  creative_new_tab_page_ad.creative_set_id = ColumnString(record, 1);
  creative_new_tab_page_ad.campaign_id = ColumnString(record, 2);
  creative_new_tab_page_ad.start_at_timestamp = ColumnInt64(record, 3);
  creative_new_tab_page_ad.end_at_timestamp = ColumnInt64(record, 4);
  creative_new_tab_page_ad.daily_cap = ColumnInt(record, 5);
  creative_new_tab_page_ad.advertiser_id = ColumnString(record, 6);
  creative_new_tab_page_ad.priority = ColumnInt(record, 7);
  creative_new_tab_page_ad.conversion = ColumnBool(record, 8);
  creative_new_tab_page_ad.per_day = ColumnInt(record, 9);
  creative_new_tab_page_ad.total_max = ColumnInt(record, 10);
  creative_new_tab_page_ad.category = ColumnString(record, 11);
  creative_new_tab_page_ad.geo_targets.push_back(ColumnString(record, 12));
  creative_new_tab_page_ad.target_url = ColumnString(record, 13);
  creative_new_tab_page_ad.company_name = ColumnString(record, 14);
  creative_new_tab_page_ad.alt = ColumnString(record, 15);
  creative_new_tab_page_ad.ptr = ColumnDouble(record, 16);

  CreativeDaypartInfo daypart;
  daypart.dow = ColumnString(record, 17);
  daypart.start_minute = ColumnInt(record, 18);
  daypart.end_minute = ColumnInt(record, 19);
  creative_new_tab_page_ad.dayparts.push_back(daypart);

  return creative_new_tab_page_ad;
}

void CreativeNewTabPageAds::CreateTableV3(
    DBTransaction* transaction) {
  DCHECK(transaction);

  const std::string query = base::StringPrintf(
      "CREATE TABLE %s "
          "(creative_instance_id TEXT NOT NULL PRIMARY KEY UNIQUE "
              "ON CONFLICT REPLACE, "
          "creative_set_id TEXT NOT NULL, "
          "campaign_id TEXT NOT NULL, "
          "company_name TEXT NOT NULL, "
          "alt TEXT NOT NULL)",
      get_table_name().c_str());

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::EXECUTE;
  command->command = query;

  transaction->commands.push_back(std::move(command));
}

void CreativeNewTabPageAds::MigrateToV3(
    DBTransaction* transaction) {
  DCHECK(transaction);

  util::Drop(transaction, get_table_name());

  CreateTableV3(transaction);
}

}  // namespace table
}  // namespace database
}  // namespace ads
