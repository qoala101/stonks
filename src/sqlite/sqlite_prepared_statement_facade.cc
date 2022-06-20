#include "sqlite_prepared_statement_facade.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <cstdint>
#include <gsl/assert>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace stonks::sqlite {
namespace {
[[nodiscard]] auto Logger() -> auto & {
  static auto logger = spdlog::stdout_color_mt("SqlitePreparedStatementFacade");
  return *logger;
}

void BindParam(sqlite3_stmt &statement, int index, const sqldb::Value &value) {
  Expects(index > 0);

  if (value.IsNull()) {
    return;
  }

  auto result_code = SQLITE_OK;

  switch (value.GetType()) {
    case sqldb::DataType::kInt:
      result_code = sqlite3_bind_int(&statement, index, value.GetInt());
      break;
    case sqldb::DataType::kInt64:
      result_code = sqlite3_bind_int64(&statement, index, value.GetInt64());
      break;
    case sqldb::DataType::kDouble:
      result_code = sqlite3_bind_double(&statement, index, value.GetDouble());
      break;
    case sqldb::DataType::kString: {
      const auto &string = value.GetString();
      result_code =
          sqlite3_bind_text(&statement, index, string.c_str(),
                            static_cast<int>(string.length()), nullptr);
      break;
    }
  }

  if (result_code != SQLITE_OK) {
    throw std::runtime_error{"Couldn't bind parameter to prepared statement: " +
                             std::to_string(result_code)};
  }
}

auto GetValue(sqlite3_stmt &statement, int index, sqldb::DataType type)
    -> sqldb::Value {
  Expects(index >= 0);

  if (sqlite3_column_type(&statement, index) == SQLITE_NULL) {
    return {};
  }

  switch (type) {
    case sqldb::DataType::kInt:
      return sqldb::Value{sqlite3_column_int(&statement, index)};
    case sqldb::DataType::kInt64:
      return sqldb::Value{int64_t{sqlite3_column_int64(&statement, index)}};
    case sqldb::DataType::kDouble:
      return sqldb::Value{sqlite3_column_double(&statement, index)};
    case sqldb::DataType::kString:
      // NOLINTNEXTLINE(*-reinterpret-cast)
      return sqldb::Value{reinterpret_cast<const char *>(
          sqlite3_column_text(&statement, index))};
  }
}
}  // namespace

SqlitePreparedStatementFacade::SqlitePreparedStatementFacade(
    sqlite3_stmt &sqlite_statement)
    : sqlite_statement_{sqlite_statement} {}

void SqlitePreparedStatementFacade::Reset() {
  const auto result_code = sqlite3_reset(&sqlite_statement_);

  if (result_code != SQLITE_OK) {
    throw std::runtime_error{"Couldn't reset prepared statement: " +
                             std::to_string(result_code)};
  }
}

void SqlitePreparedStatementFacade::BindParams(
    const std::vector<sqldb::Value> &params) {
  for (auto i = 0; i < params.size(); ++i) {
    BindParam(sqlite_statement_, i + 1, params[i]);
  }
}

auto SqlitePreparedStatementFacade::Step() -> ResultCode {
  return sqlite3_step(&sqlite_statement_);
}

auto SqlitePreparedStatementFacade::GetStepValues(
    const std::vector<sqldb::DataType> &value_types)
    -> std::vector<sqldb::Value> {
  const auto num_values = value_types.size();

  auto values = std::vector<sqldb::Value>{};
  values.reserve(num_values);

  for (auto i = 0; i < num_values; ++i) {
    values.emplace_back(GetValue(sqlite_statement_, i, value_types[i]));
  }

  Ensures(values.size() == value_types.size());
  return values;
}
}  // namespace stonks::sqlite