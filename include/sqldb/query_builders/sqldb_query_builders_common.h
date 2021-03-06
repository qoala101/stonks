#ifndef STONKS_SQLDB_QUERY_BUILDER_FACADE_SQLDB_QUERY_BUILDERS_COMMON_H_
#define STONKS_SQLDB_QUERY_BUILDER_FACADE_SQLDB_QUERY_BUILDERS_COMMON_H_

#include <variant>
#include <vector>

#include "sqldb_types.h"

/**
 * @file Symbols used by query builders.
 */
namespace stonks::sqldb {
/**
 * @brief Variant of columns specification.
 */
struct AllColumnsType {};

/**
 * @brief Stores table value in query builders.
 */
using TableVariant = std::variant<std::monostate, Table, TableDefinition>;

/**
 * @brief Stores columns value in query builders.
 */
using ColumnsVariant =
    std::variant<std::monostate, std::vector<Column>, AllColumnsType>;

/**
 * @brief Get table from variant.
 */
[[nodiscard]] auto GetTable(const TableVariant &table) -> const Table &;

/**
 * @brief Get columns from stored values.
 */
[[nodiscard]] auto GetColumns(const TableVariant &table,
                              const ColumnsVariant &columns)
    -> std::vector<Column>;

/**
 * @brief Get columns from their definitions.
 */
[[nodiscard]] auto GetColumns(
    const ConstView<ColumnDefinition> &column_definitions)
    -> std::vector<Column>;
}  // namespace stonks::sqldb

#endif  // STONKS_SQLDB_QUERY_BUILDER_FACADE_SQLDB_QUERY_BUILDERS_COMMON_H_
