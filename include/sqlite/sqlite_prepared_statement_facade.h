#ifndef STONKS_SQLITE_SQLITE_PREPARED_STATEMENT_FACADE_H_
#define STONKS_SQLITE_SQLITE_PREPARED_STATEMENT_FACADE_H_

#include <sqlite3.h>

#include <vector>

#include "not_null.hpp"
#include "sqldb_enums.h"
#include "sqldb_value.h"

namespace stonks::sqlite {
/**
 * @brief Convenience API for SQLite prepared statement.
 */
class SqlitePreparedStatementFacade {
 public:
  /**
   * @brief SQLite result code.
   */
  using ResultCode = int;

  explicit SqlitePreparedStatementFacade(
      cpp::not_null<sqlite3_stmt *> sqlite_statement);

  /**
   * @brief Resets prepared statement.
   * @remark Should be called before new parameters binding.
   */
  void Reset();

  /**
   * @brief Binds parameters in the order they defined in the statement.
   */
  void BindParams(const std::vector<sqldb::Value> &params);

  /**
   * @brief Executes the next statement step.
   * @return SQLite result code.
   */
  auto Step() -> ResultCode;

  /**
   * @brief Gets the values of provided types from the current step in the order
   * they are refined in the statement.
   */
  [[nodiscard]] auto GetStepValues(
      const std::vector<sqldb::DataType> &value_types) const
      -> std::vector<sqldb::Value>;

  /**
   * @brief Finalizes the statement.
   * @remark Other methods should not be called after this.
   */
  void Finalize();

 private:
  sqlite3_stmt *sqlite_statement_;
};
}  // namespace stonks::sqlite

#endif  // STONKS_SQLITE_SQLITE_PREPARED_STATEMENT_FACADE_H_
