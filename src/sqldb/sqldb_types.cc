#include "sqldb_types.h"

#include <gsl/assert>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/basic_iterator.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/view.hpp>

#include "not_null.hpp"

namespace stonks::sqldb {
auto TableDefinition::GetColumnDefinition(const Column &column) const
    -> const ColumnDefinition & {
  const auto iter =
      ranges::find_if(columns, [&column](const auto &column_definition) {
        return column_definition.column == column;
      });
  Expects(iter != columns.end());
  return *iter.base();
}

[[nodiscard]] auto TableDefinition::GetColumnDefinitions(
    const std::vector<Column> &columns) const -> ConstView<ColumnDefinition> {
  auto column_definitions =
      columns | ranges::views::transform([this](const auto &column) {
        return cpp::assume_not_null(&GetColumnDefinition(column));
      }) |
      ranges::to_vector;
  Ensures(column_definitions.size() == columns.size());
  return column_definitions;
}
}  // namespace stonks::sqldb