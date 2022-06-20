#include "sqlite_update_statement.h"

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <sqlite3.h>

#include <gsl/assert>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "sqlite_prepared_statement_facade.h"
#include "sqlite_prepared_statement_handle.h"

namespace stonks::sqlite {
SqliteUpdateStatement::SqliteUpdateStatement(
    SqlitePreparedStatementHandle &&prepared_statement_handle)
    : prepared_statement_handle_{std::move(prepared_statement_handle)} {
  Expects(prepared_statement_handle_.IsValid());
}

void SqliteUpdateStatement::Execute(const std::vector<sqldb::Value> &params) {
  auto sqlite_statement = prepared_statement_handle_.GetSqliteStatement();
  Expects(sqlite_statement != nullptr);

  auto prepared_statement_facade =
      SqlitePreparedStatementFacade{*sqlite_statement};
  prepared_statement_facade.Reset();
  prepared_statement_facade.BindParams(params);

  const auto result_code = prepared_statement_facade.Step();

  if (result_code != SQLITE_DONE) {
    throw std::runtime_error{"Unexpected update statement result: {}" +
                             std::to_string(result_code)};
  }
}
}  // namespace stonks::sqlite