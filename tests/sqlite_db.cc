#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include "not_null.hpp"
#include "sqldb_db.h"
#include "sqldb_query_builder.h"
#include "sqldb_query_builder_facade.h"
#include "sqldb_types.h"
#include "sqldb_value.h"
#include "sqlite_db_factory.h"
#include "utils.h"

namespace {
const auto kTestDbFileName = "sqlite_db_test.db";

auto db_factory = std::unique_ptr<stonks::sqldb::IDbFactory>{};
auto db = std::unique_ptr<stonks::sqldb::IDb>{};
auto query_builder = std::shared_ptr<stonks::sqldb::IQueryBuilder>{};
auto query_builder_facade = std::optional<stonks::sqldb::QueryBuilderFacade>{};

TEST(SqliteDb, CreateAndDropTable) {
  static_cast<void>(std::filesystem::remove(kTestDbFileName));
  db_factory = std::make_unique<stonks::sqlite::SqliteDbFactory>();
  db = db_factory->LoadDbFromFile(kTestDbFileName).as_nullable();
  query_builder = db_factory->CreateQueryBuilder().as_nullable();
  query_builder_facade.emplace(cpp::assume_not_null(query_builder));

  const auto table = stonks::sqldb::Table{"TestTable"};
  const auto table_definition = stonks::sqldb::TableDefinition{
      .table = table,
      .columns = {stonks::sqldb::ColumnDefinition{
                      .column = "IntTrue",
                      .data_type = stonks::sqldb::DataType::kInt,
                      .primary_key = true,
                      .auto_increment = true,
                      .unique = true},
                  stonks::sqldb::ColumnDefinition{
                      .column = "TextFalse",
                      .data_type = stonks::sqldb::DataType::kString}}};
  db->PrepareStatement(
        query_builder->BuildCreateTableIfNotExistsQuery(table_definition))
      ->Execute();
  db->PrepareStatement(
        query_builder->BuildCreateTableIfNotExistsQuery(table_definition))
      ->Execute();
  db->PrepareStatement(query_builder->BuildDropTableQuery(table))->Execute();
  EXPECT_ANY_THROW(
      db->PrepareStatement(query_builder->BuildDropTableQuery(table))
          ->Execute());
}

const auto kAssetTableDefinition = stonks::sqldb::TableDefinition{
    .table = stonks::sqldb::Table{"Asset"},
    .columns = {
        stonks::sqldb::ColumnDefinition{
            .column = "id",
            .data_type = stonks::sqldb::DataType::kInt64,
            .primary_key = true,
            .auto_increment = true,
            .unique = true},
        stonks::sqldb::ColumnDefinition{
            .column = "name",
            .data_type = stonks::sqldb::DataType::kString,
            .unique = true},
    }};

TEST(SqliteDb, InsertAndSelect) {
  db->PrepareStatement(
        query_builder->BuildCreateTableIfNotExistsQuery(kAssetTableDefinition))
      ->Execute();

  auto insert_statement =
      db->PrepareStatement(query_builder_facade->Insert()
                               .IntoTable(kAssetTableDefinition)
                               .IntoColumns({"name"})
                               .Build());
  insert_statement->Execute({std::string_view{"BTC"}});
  insert_statement->Execute({std::string_view{"ETH"}});
  insert_statement->Execute({std::string_view{"USDT"}});

  auto select_statement =
      db->PrepareStatement(query_builder_facade->Select()
                               .AllColumns()
                               .FromTable(kAssetTableDefinition.table)
                               .Build(),
                           kAssetTableDefinition);

  const auto rows = select_statement->Execute();
  EXPECT_EQ(rows.GetSize(), 3);
  EXPECT_EQ(rows.GetColumnValues("id")[0].GetInt64(), 1);
  EXPECT_EQ(rows.GetColumnValues("name")[0].GetString(), "BTC");
  EXPECT_EQ(rows.GetColumnValues("id")[1].GetInt64(), 2);
  EXPECT_EQ(rows.GetColumnValues("name")[1].GetString(), "ETH");
  EXPECT_EQ(rows.GetColumnValues("id")[2].GetInt64(), 3);
  EXPECT_EQ(rows.GetColumnValues("name")[2].GetString(), "USDT");
}

TEST(SqliteDb, InsertNull) {
  auto insert_statement = db->PrepareStatement(
      query_builder_facade->Insert().IntoTable(kAssetTableDefinition).Build());
  EXPECT_ANY_THROW(insert_statement->Execute());
}

const auto kSymbolTableDefinition = stonks::sqldb::TableDefinition{
    .table = "Symbol",
    .columns = {stonks::sqldb::ColumnDefinition{
                    .column = "id",
                    .data_type = stonks::sqldb::DataType::kInt64,
                    .primary_key = true,
                    .auto_increment = true,
                    .unique = true},
                stonks::sqldb::ColumnDefinition{
                    .column = "base_asset_id",
                    .data_type = stonks::sqldb::DataType::kInt64,
                    .foreign_key = stonks::sqldb::ForeignKey{.table = "Asset",
                                                             .column = "id"}},
                stonks::sqldb::ColumnDefinition{
                    .column = "quote_asset_id",
                    .data_type = stonks::sqldb::DataType::kInt64,
                    .foreign_key = stonks::sqldb::ForeignKey{.table = "Asset",
                                                             .column = "id"}}}};

const auto kSymbolPriceTableDefinition = stonks::sqldb::TableDefinition{
    .table = "SymbolPrice",
    .columns = {
        stonks::sqldb::ColumnDefinition{
            .column = "symbol_id",
            .data_type = stonks::sqldb::DataType::kInt64,
            .foreign_key =
                stonks::sqldb::ForeignKey{.table = "Symbol", .column = "id"}},
        stonks::sqldb::ColumnDefinition{
            .column = "time", .data_type = stonks::sqldb::DataType::kInt64},
        stonks::sqldb::ColumnDefinition{
            .column = "price", .data_type = stonks::sqldb::DataType::kDouble}}};

TEST(SqliteDb, ForeignKey) {
  db->PrepareStatement(
        query_builder->BuildCreateTableIfNotExistsQuery(kSymbolTableDefinition))
      ->Execute();

  auto insert_symbol_statement = db->PrepareStatement(
      query_builder_facade->Insert()
          .IntoTable(kSymbolTableDefinition.table)
          .IntoColumns({{"base_asset_id"}, {"quote_asset_id"}})
          .Build());
  insert_symbol_statement->Execute({1, 3});
  insert_symbol_statement->Execute({2, 3});
  EXPECT_ANY_THROW(insert_symbol_statement->Execute({5, 6}));

  db->PrepareStatement(query_builder->BuildCreateTableIfNotExistsQuery(
                           kSymbolPriceTableDefinition))
      ->Execute();

  auto insert_symbol_price_statement =
      db->PrepareStatement(query_builder_facade->Insert()
                               .IntoTable(kSymbolPriceTableDefinition)
                               .Build());
  insert_symbol_price_statement->Execute(
      {1, stonks::utils::GetUnixTime().count(), 12345});
  insert_symbol_price_statement->Execute(
      {2, stonks::utils::GetUnixTime().count(), 0.12345});
}

TEST(SqliteDb, SelectJoin) {
  const auto cell_definitions = std::vector<stonks::sqldb::CellDefinition>{
      stonks::sqldb::CellDefinition{
          .column = "base_asset",
          .data_type = stonks::sqldb::DataType::kString},
      stonks::sqldb::CellDefinition{
          .column = "quote_asset",
          .data_type = stonks::sqldb::DataType::kString}};

  auto select_statement = db->PrepareStatement(
      "SELECT BaseAsset.name AS base_asset, QuoteAsset.name AS quote_asset "
      "FROM Symbol "
      "JOIN Asset AS BaseAsset ON Symbol.base_asset_id=BaseAsset.id "
      "JOIN Asset AS QuoteAsset ON Symbol.quote_asset_id=QuoteAsset.id;",
      cell_definitions);
  const auto rows = select_statement->Execute();
  EXPECT_EQ(rows.GetSize(), 2);
  EXPECT_EQ(rows.GetColumnValues("base_asset")[0].GetString(), "BTC");
  EXPECT_EQ(rows.GetColumnValues("quote_asset")[0].GetString(), "USDT");
  EXPECT_EQ(rows.GetColumnValues("base_asset")[1].GetString(), "ETH");
  EXPECT_EQ(rows.GetColumnValues("quote_asset")[1].GetString(), "USDT");
}

TEST(SqliteDb, FileWriteAndRead) {
  EXPECT_FALSE(std::filesystem::exists(kTestDbFileName));
  db->WriteToFile(kTestDbFileName);
  EXPECT_TRUE(std::filesystem::exists(kTestDbFileName));

  auto db_copy = db_factory->LoadDbFromFile(kTestDbFileName).as_nullable();

  const auto select_query = query_builder_facade->Select()
                                .AllColumns()
                                .FromTable(kSymbolPriceTableDefinition.table)
                                .Build();
  const auto db_rows =
      db->PrepareStatement(select_query, kSymbolPriceTableDefinition)
          ->Execute();
  const auto db_copy_rows =
      db_copy->PrepareStatement(select_query, kSymbolPriceTableDefinition)
          ->Execute();
  EXPECT_GT(db_rows.GetSize(), 0);
  EXPECT_EQ(db_rows, db_copy_rows);

  db_copy.reset();
  EXPECT_TRUE(std::filesystem::exists(kTestDbFileName));
}

TEST(SqliteDb, CascadeForeignKeyDelete) {
  db->PrepareStatement(query_builder_facade->Delete()
                           .FromTable(kAssetTableDefinition.table)
                           .Where("Asset.name = \"USDT\"")
                           .Build())
      ->Execute();

  auto select_statement =
      db->PrepareStatement(query_builder_facade->Select()
                               .AllColumns()
                               .FromTable(kAssetTableDefinition.table)
                               .Build(),
                           kAssetTableDefinition);
  auto rows = select_statement->Execute();
  EXPECT_EQ(rows.GetSize(), 2);

  select_statement =
      db->PrepareStatement(query_builder_facade->Select()
                               .AllColumns()
                               .FromTable(kSymbolTableDefinition.table)
                               .Build(),
                           kSymbolTableDefinition);
  rows = select_statement->Execute();
  EXPECT_EQ(rows.GetSize(), 0);

  select_statement =
      db->PrepareStatement(query_builder_facade->Select()
                               .AllColumns()
                               .FromTable(kSymbolPriceTableDefinition.table)
                               .Build(),
                           kSymbolPriceTableDefinition);
  rows = select_statement->Execute();
  EXPECT_EQ(rows.GetSize(), 0);
}
}  // namespace