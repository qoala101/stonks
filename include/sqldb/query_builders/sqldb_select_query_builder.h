#ifndef STONKS_SQLDB_QUERY_BUILDERS_SQLDB_SELECT_QUERY_BUILDER_H_
#define STONKS_SQLDB_QUERY_BUILDERS_SQLDB_SELECT_QUERY_BUILDER_H_

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "not_null.hpp"
#include "sqldb_query_builder.h"
#include "sqldb_query_builders_common.h"
#include "sqldb_types.h"

namespace stonks::sqldb {
/**
 * @brief Part of query builder facade.
 */
class SelectQueryBuilder {
 public:
  explicit SelectQueryBuilder(
      cpp::not_null<std::shared_ptr<IQueryBuilder>> query_builder);

  auto Columns(std::vector<Column> columns) -> SelectQueryBuilder &;
  auto Columns(const ConstView<ColumnDefinition> &column_definitions)
      -> SelectQueryBuilder &;
  auto AllColumns() -> SelectQueryBuilder &;
  auto FromTable(Table table) -> SelectQueryBuilder &;
  auto FromTable(const TableDefinition &table_definition)
      -> SelectQueryBuilder &;
  auto Where(std::string_view where_clause) -> SelectQueryBuilder &;
  auto And(std::string_view where_clause) -> SelectQueryBuilder &;
  auto Or(std::string_view where_clause) -> SelectQueryBuilder &;
  auto Limit(int limit) -> SelectQueryBuilder &;
  auto Limited() -> SelectQueryBuilder &;

  [[nodiscard]] auto Build() const -> std::string;

 private:
  struct LimitedType {};

  using LimitVariant = std::variant<std::monostate, int, LimitedType>;

  cpp::not_null<std::shared_ptr<IQueryBuilder>> query_builder_;

  std::optional<Table> table_{};
  ColumnsVariant columns_{};
  std::optional<std::string> where_clause_{};
  LimitVariant limit_{};
};
}  // namespace stonks::sqldb

#endif  // STONKS_SQLDB_QUERY_BUILDERS_SQLDB_SELECT_QUERY_BUILDER_H_
