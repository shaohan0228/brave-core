/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/database/tables/creative_new_tab_page_ads_database_table.h"

#include <memory>
#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/test/task_environment.h"
#include "brave/components/l10n/browser/locale_helper_mock.h"
#include "net/http/http_status_code.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/bundle/creative_new_tab_page_ad_info.h"
#include "bat/ads/internal/container_util.h"
#include "bat/ads/internal/database/database_initialize.h"
#include "bat/ads/internal/platform/platform_helper_mock.h"
#include "bat/ads/internal/time_util.h"
#include "bat/ads/internal/unittest_util.h"
#include "bat/ads/pref_names.h"

// npm run test -- brave_unit_tests --filter=BatAds*

using ::testing::NiceMock;
using ::testing::Return;

namespace ads {

class BatAdsCreativeNewTabPageAdsDatabaseTableTest : public ::testing::Test {
 protected:
  BatAdsCreativeNewTabPageAdsDatabaseTableTest()
      : task_environment_(base::test::TaskEnvironment::TimeSource::MOCK_TIME),
        ads_client_mock_(std::make_unique<NiceMock<AdsClientMock>>()),
        ads_(std::make_unique<AdsImpl>(ads_client_mock_.get())),
        locale_helper_mock_(std::make_unique<
            NiceMock<brave_l10n::LocaleHelperMock>>()),
        platform_helper_mock_(std::make_unique<
            NiceMock<PlatformHelperMock>>()),
        database_table_(std::make_unique<
            database::table::CreativeNewTabPageAds>(ads_.get())) {
    // You can do set-up work for each test here

    brave_l10n::LocaleHelper::GetInstance()->set_for_testing(
        locale_helper_mock_.get());

    PlatformHelper::GetInstance()->set_for_testing(platform_helper_mock_.get());
  }

  ~BatAdsCreativeNewTabPageAdsDatabaseTableTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)

    ASSERT_TRUE(temp_dir_.CreateUniqueTempDir());
    const base::FilePath path = temp_dir_.GetPath();

    database_ = std::make_unique<Database>(path.AppendASCII("database.sqlite"));
    MockRunDBTransaction(ads_client_mock_, database_);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }

  // Objects declared here can be used by all tests in the test case

  void CreateOrOpenDatabase() {
    database::Initialize initialize(ads_.get());
    initialize.CreateOrOpen([](
        const Result result) {
      ASSERT_EQ(Result::SUCCESS, result);
    });
  }

  void SaveDatabase(
      const CreativeNewTabPageAdList creative_new_tab_page_ads) {
    database_table_->Save(creative_new_tab_page_ads, [](
        const Result result) {
      ASSERT_EQ(Result::SUCCESS, result);
    });
  }

  base::test::TaskEnvironment task_environment_;

  base::ScopedTempDir temp_dir_;

  std::unique_ptr<AdsClientMock> ads_client_mock_;
  std::unique_ptr<AdsImpl> ads_;
  std::unique_ptr<brave_l10n::LocaleHelperMock> locale_helper_mock_;
  std::unique_ptr<PlatformHelperMock> platform_helper_mock_;
  std::unique_ptr<database::table::CreativeNewTabPageAds> database_table_;
  std::unique_ptr<Database> database_;
};

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    SaveEmptyCreativeNewTabPageAds) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads = {};

  // Act
  SaveDatabase(creative_new_tab_page_ads);

  // Assert
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    SaveCreativeNewTabPageAds) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPast();
  info_1.end_at_timestamp = DistantFuture();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.category = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = { "US" };
  info_1.target_url = "https://brave.com";
  info_1.company_name = "Test Ad 1 Title";
  info_1.alt = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_1);

  CreativeNewTabPageAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPast();
  info_2.end_at_timestamp = DistantFuture();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.category = "Technology & Computing-Software";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = { "US" };
  info_2.target_url = "https://brave.com";
  info_2.company_name = "Test Ad 2 Title";
  info_2.alt = "Test Ad 2 Body";
  info_2.ptr = 0.8;
  creative_new_tab_page_ads.push_back(info_2);

  // Act
  SaveDatabase(creative_new_tab_page_ads);

  // Assert
  const CreativeNewTabPageAdList expected_creative_new_tab_page_ads =
      creative_new_tab_page_ads;

  const std::vector<std::string> categories = {
    "Technology & Computing-Software"
  };

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    SaveCreativeNewTabPageAdsInBatches) {
  // Arrange
  database_table_->set_batch_size(2);

  CreateOrOpenDatabase();

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeNewTabPageAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPast();
  info_1.end_at_timestamp = DistantFuture();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.category = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = { "US" };
  info_1.target_url = "https://brave.com";
  info_1.company_name = "Test Ad 1 Title";
  info_1.alt = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_1);

  CreativeNewTabPageAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPast();
  info_2.end_at_timestamp = DistantFuture();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.category = "Technology & Computing-Software";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = { "US" };
  info_2.target_url = "https://brave.com";
  info_2.company_name = "Test Ad 2 Title";
  info_2.alt = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_2);

  CreativeNewTabPageAdInfo info_3;
  info_3.creative_instance_id = "a1ac44c2-675f-43e6-ab6d-500614cafe63";
  info_3.creative_set_id = "5800049f-cee5-4bcb-90c7-85246d5f5e7c";
  info_3.campaign_id = "3d62eca2-324a-4161-a0c5-7d9f29d10ab0";
  info_3.start_at_timestamp = DistantPast();
  info_3.end_at_timestamp = DistantFuture();
  info_3.daily_cap = 1;
  info_3.advertiser_id = "9a11b60f-e29d-4446-8d1f-318311e36e0a";
  info_3.priority = 2;
  info_3.per_day = 3;
  info_3.total_max = 4;
  info_3.category = "Technology & Computing-Software";
  info_3.dayparts.push_back(daypart_info);
  info_3.geo_targets = { "US" };
  info_3.target_url = "https://brave.com";
  info_3.company_name = "Test Ad 3 Title";
  info_3.alt = "Test Ad 3 Body";
  info_3.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_3);

  // Act
  SaveDatabase(creative_new_tab_page_ads);

  // Assert
  const CreativeNewTabPageAdList expected_creative_new_tab_page_ads =
      creative_new_tab_page_ads;

  const std::vector<std::string> categories = {
    "Technology & Computing-Software"
  };

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    DoNotSaveDuplicateCreativeNewTabPageAds) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPast();
  info.end_at_timestamp = DistantFuture();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.category = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = { "US" };
  info.target_url = "https://brave.com";
  info.company_name = "Test Ad 1 Title";
  info.alt = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info);

  SaveDatabase(creative_new_tab_page_ads);

  // Act
  SaveDatabase(creative_new_tab_page_ads);

  // Assert
  const CreativeNewTabPageAdList expected_creative_new_tab_page_ads =
      creative_new_tab_page_ads;

  const std::vector<std::string> categories = {
    "Technology & Computing-Software"
  };

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetForCategories) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPast();
  info_1.end_at_timestamp = DistantFuture();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.category = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = { "US" };
  info_1.target_url = "https://brave.com";
  info_1.company_name = "Test Ad 1 Title";
  info_1.alt = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_1);

  CreativeNewTabPageAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPast();
  info_2.end_at_timestamp = DistantFuture();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.category = "Technology & Computing-Software";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = { "US" };
  info_2.target_url = "https://brave.com";
  info_2.company_name = "Test Ad 2 Title";
  info_2.alt = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_2);

  SaveDatabase(creative_new_tab_page_ads);

  // Act

  // Assert
  const CreativeNewTabPageAdList expected_creative_new_tab_page_ads =
      creative_new_tab_page_ads;

  const std::vector<std::string> categories = {
    "Technology & Computing-Software"
  };

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetCreativeNewTabPageAdsForCreativeInstanceId) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPast();
  info.end_at_timestamp = DistantFuture();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.category = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = { "US" };
  info.target_url = "https://brave.com";
  info.company_name = "Test Ad 1 Title";
  info.alt = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info);

  SaveDatabase(creative_new_tab_page_ads);

  // Act

  // Assert
  const CreativeNewTabPageAdInfo expected_creative_new_tab_page_ad = info;

  const std::string creative_instance_id =
      "3519f52c-46a4-4c48-9c2b-c264c0067f04";

  database_table_->GetForCreativeInstanceId(creative_instance_id,
      [&expected_creative_new_tab_page_ad](
          const Result result,
          const std::string& creative_instance_id,
          const CreativeNewTabPageAdInfo& creative_new_tab_page_ad) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_EQ(expected_creative_new_tab_page_ad, creative_new_tab_page_ad);
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetCreativeNewTabPageAdsForNonExistentCreativeInstanceId) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPast();
  info.end_at_timestamp = DistantFuture();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.category = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = { "US" };
  info.target_url = "https://brave.com";
  info.company_name = "Test Ad 1 Title";
  info.alt = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info);

  SaveDatabase(creative_new_tab_page_ads);

  // Act

  // Assert
  const std::string creative_instance_id =
      "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

  database_table_->GetForCreativeInstanceId(creative_instance_id, [](
      const Result result,
      const std::string& creative_instance_id,
      const CreativeNewTabPageAdInfo& creative_new_tab_page_ad) {
    EXPECT_EQ(Result::FAILED, result);
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetCreativeNewTabPageAdsForEmptyCategories) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPast();
  info.end_at_timestamp = DistantFuture();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.category = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = { "US" };
  info.target_url = "https://brave.com";
  info.company_name = "Test Ad 1 Title";
  info.alt = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info);

  SaveDatabase(creative_new_tab_page_ads);

  // Act

  // Assert
  const CreativeNewTabPageAdList expected_creative_new_tab_page_ads = {};

  const std::vector<std::string> categories = {};

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetCreativeNewTabPageAdsForNonExistentCategory) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPast();
  info.end_at_timestamp = DistantFuture();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.category = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = { "US" };
  info.target_url = "https://brave.com";
  info.company_name = "Test Ad 1 Title";
  info.alt = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info);

  SaveDatabase(creative_new_tab_page_ads);

  // Act

  // Assert
  const CreativeNewTabPageAdList expected_creative_new_tab_page_ads = {};

  const std::vector<std::string> categories = {
    "Food & Drink"
  };

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetCreativeNewTabPageAdsFromMultipleCategories) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPast();
  info_1.end_at_timestamp = DistantFuture();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.category = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = { "US" };
  info_1.target_url = "https://brave.com";
  info_1.company_name = "Test Ad 1 Title";
  info_1.alt = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_1);

  CreativeNewTabPageAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPast();
  info_2.end_at_timestamp = DistantFuture();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.category = "Food & Drink";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = { "US" };
  info_2.target_url = "https://brave.com";
  info_2.company_name = "Test Ad 2 Title";
  info_2.alt = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_2);

  CreativeNewTabPageAdInfo info_3;
  info_3.creative_instance_id = "a1ac44c2-675f-43e6-ab6d-500614cafe63";
  info_3.creative_set_id = "5800049f-cee5-4bcb-90c7-85246d5f5e7c";
  info_3.campaign_id = "3d62eca2-324a-4161-a0c5-7d9f29d10ab0";
  info_3.start_at_timestamp = DistantPast();
  info_3.end_at_timestamp = DistantFuture();
  info_3.daily_cap = 1;
  info_3.advertiser_id = "9a11b60f-e29d-4446-8d1f-318311e36e0a";
  info_3.priority = 2;
  info_3.per_day = 3;
  info_3.total_max = 4;
  info_3.category = "Automobiles";
  info_3.dayparts.push_back(daypart_info);
  info_3.geo_targets = { "US" };
  info_3.target_url = "https://brave.com";
  info_3.company_name = "Test Ad 3 Title";
  info_3.alt = "Test Ad 3 Body";
  info_3.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_3);

  SaveDatabase(creative_new_tab_page_ads);

  // Act

  // Assert
  CreativeNewTabPageAdList expected_creative_new_tab_page_ads;
  expected_creative_new_tab_page_ads.push_back(info_1);
  expected_creative_new_tab_page_ads.push_back(info_2);

  const std::vector<std::string> categories = {
    "Technology & Computing-Software",
    "Food & Drink"
  };

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetNonExpiredCreativeNewTabPageAds) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPast();
  info_1.end_at_timestamp = static_cast<int64_t>(base::Time::Now().ToDoubleT());
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.category = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = { "US" };
  info_1.target_url = "https://brave.com";
  info_1.company_name = "Test Ad 1 Title";
  info_1.alt = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_1);

  CreativeNewTabPageAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPast();
  info_2.end_at_timestamp = DistantFuture();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.category = "Technology & Computing-Software";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = { "US" };
  info_2.target_url = "https://brave.com";
  info_2.company_name = "Test Ad 2 Title";
  info_2.alt = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_2);

  SaveDatabase(creative_new_tab_page_ads);

  // Act
  task_environment_.FastForwardBy(base::TimeDelta::FromHours(1));

  // Assert
  CreativeNewTabPageAdList expected_creative_new_tab_page_ads;
  expected_creative_new_tab_page_ads.push_back(info_2);

  const std::vector<std::string> categories = {
    "Technology & Computing-Software"
  };

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetCreativeNewTabPageAdsMatchingCaseInsensitiveCategories) {
  // Arrange
  CreateOrOpenDatabase();

  CreativeNewTabPageAdList creative_new_tab_page_ads;

  CreativeDaypartInfo daypart_info;
  CreativeNewTabPageAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPast();
  info_1.end_at_timestamp = DistantFuture();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.category = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = { "US" };
  info_1.target_url = "https://brave.com";
  info_1.company_name = "Test Ad 1 Title";
  info_1.alt = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_1);

  CreativeNewTabPageAdInfo info_2;
  info_2.creative_instance_id = "a1ac44c2-675f-43e6-ab6d-500614cafe63";
  info_2.creative_set_id = "5800049f-cee5-4bcb-90c7-85246d5f5e7c";
  info_2.campaign_id = "3d62eca2-324a-4161-a0c5-7d9f29d10ab0";
  info_2.start_at_timestamp = DistantPast();
  info_2.end_at_timestamp = DistantFuture();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "9a11b60f-e29d-4446-8d1f-318311e36e0a";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.category = "Food & Drink";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = { "US" };
  info_2.target_url = "https://brave.com";
  info_2.company_name = "Test Ad 2 Title";
  info_2.alt = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_new_tab_page_ads.push_back(info_2);

  SaveDatabase(creative_new_tab_page_ads);

  // Act

  // Assert
  CreativeNewTabPageAdList expected_creative_new_tab_page_ads;
  expected_creative_new_tab_page_ads.push_back(info_2);

  const std::vector<std::string> categories = {
    "FoOd & DrInK"
  };

  database_table_->GetForCategories(categories,
      [&expected_creative_new_tab_page_ads](
          const Result result,
          const classification::CategoryList& categories,
          const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(CompareAsSets(expected_creative_new_tab_page_ads,
        creative_new_tab_page_ads));
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    GetCreativeNewTabPageAdsFromCatalogEndpoint) {
  // Arrange
  SetBuildChannel(false, "test");

  ON_CALL(*locale_helper_mock_, GetLocale())
      .WillByDefault(Return("en-US"));

  MockPlatformHelper(platform_helper_mock_, PlatformType::kMacOS);

  ads_->OnWalletUpdated("c387c2d8-a26d-4451-83e4-5c0c6fd942be",
      "5BEKM1Y7xcRSg/1q8in/+Lki2weFZQB+UMYZlRw8ql8=");

  MockLoad(ads_client_mock_);
  MockLoadUserModelForId(ads_client_mock_);
  MockLoadResourceForId(ads_client_mock_);
  MockSave(ads_client_mock_);

  MockPrefs(ads_client_mock_);

  const URLEndpoints endpoints = {
    {
      "/v5/catalog", {
        {
          net::HTTP_OK, "/catalog.json"
        }
      }
    }
  };

  MockUrlRequest(ads_client_mock_, endpoints);

  Initialize(ads_);

  // Act

  // Assert
  const std::vector<std::string> categories = {
    "Technology & Computing"
  };

  database_table_->GetForCategories(categories, [](
      const Result result,
      const classification::CategoryList& categories,
      const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    EXPECT_EQ(Result::SUCCESS, result);
    EXPECT_EQ(1UL, creative_new_tab_page_ads.size());
  });
}

TEST_F(BatAdsCreativeNewTabPageAdsDatabaseTableTest,
    TableName) {
  // Arrange

  // Act
  const std::string table_name = database_table_->get_table_name();

  // Assert
  const std::string expected_table_name = "creative_new_tab_page_ads";
  EXPECT_EQ(expected_table_name, table_name);
}

}  // namespace ads
