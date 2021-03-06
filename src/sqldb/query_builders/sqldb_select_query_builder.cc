#include "sqldb_select_query_builder.h"

#include <gsl/assert>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "sqldb_query_builders_common.h"
#include "sqldb_types.h"

namespace stonks::sqldb {
SelectQueryBuilder::SelectQueryBuilder(
    cpp::not_null<std::shared_ptr<IQueryBuilder>> query_builder)
    : query_builder_{std::move(query_builder)} {}

auto SelectQueryBuilder::Columns(std::vector<Column> columns)
    -> SelectQueryBuilder & {
  Expects(std::holds_alternative<std::monostate>(columns_));
  columns_.emplace<std::vector<Column>>(std::move(columns));
  return *this;
}

auto SelectQueryBuilder::Columns(
    const ConstView<ColumnDefinition> &column_definitions)
    -> SelectQueryBuilder & {
  return Columns(GetColumns(column_definitions));
}

auto SelectQueryBuilder::AllColumns() -> SelectQueryBuilder & {
  Expects(std::holds_alternative<std::monostate>(columns_));
  columns_.emplace<AllColumnsType>();
  return *this;
}

auto SelectQueryBuilder::FromTable(Table table) -> SelectQueryBuilder & {
  Expects(!table_.has_value());
  table_.emplace(std::move(table));
  return *this;
}

auto SelectQueryBuilder::FromTable(const TableDefinition &table_definition)
    -> SelectQueryBuilder & {
  table_.emplace(table_definition.table);
  return *this;
}

auto SelectQueryBuilder::Where(std::string_view where_clause)
    -> SelectQueryBuilder & {
  Expects(!where_clause_.has_value());
  where_clause_.emplace(std::string{"WHERE "} + where_clause.data());
  return *this;
}

auto SelectQueryBuilder::And(std::string_view where_clause)
    -> SelectQueryBuilder & {
  Expects(where_clause_.has_value());
  (*where_clause_) += std::string{" AND "} + where_clause.data();
  return *this;
}
auto SelectQueryBuilder::Or(std::string_view where_clause)
    -> SelectQueryBuilder & {
  Expects(where_clause_.has_value());
  (*where_clause_) += std::string{" OR "} + where_clause.data();
  return *this;
}

auto SelectQueryBuilder::Limit(int limit) -> SelectQueryBuilder & {
  Expects(std::holds_alternative<std::monostate>(limit_));
  limit_.emplace<int>(limit);
  return *this;
}

auto SelectQueryBuilder::Limited() -> SelectQueryBuilder & {
  Expects(std::holds_alternative<std::monostate>(limit_));
  limit_.emplace<LimitedType>();
  return *this;
}

auto SelectQueryBuilder::Build() const -> std::string {
  Expects(table_.has_value());

  const auto *const columns = std::visit(
      [](const auto &variant) -> const std::vector<Column> * {
        using T = std::decay_t<decltype(variant)>;

        if constexpr (std::is_same_v<T, std::vector<Column>>) {
          return &variant;
        }

        if constexpr (std::is_same_v<T, AllColumnsType>) {
          return nullptr;
        }

        Expects(false);
      },
      columns_);

  const auto limit = std::visit(
      [](const auto &variant) -> std::string {
        using T = std::decay_t<decltype(variant)>;

        if constexpr (std::is_same_v<T, int>) {
          return " LIMIT " + std::to_string(variant);
        }

        if constexpr (std::is_same_v<T, LimitedType>) {
          return " LIMIT ?";
        }

        return {};
      },
      limit_);

  return query_builder_->BuildSelectQuery(
      *table_, columns, where_clause_.value_or(std::string{}) + limit);
}
}  // namespace stonks::sqldb