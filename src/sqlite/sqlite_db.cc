#include "sqlite_db.h"

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <sqlite3.h>

#include <memory>
#include <utility>

#include "not_null.hpp"
#include "sqldb_enums_to_string.h"  // IWYU pragma: keep
#include "sqldb_select_statement.h"
#include "sqldb_update_statement.h"
#include "sqlite_db_facade.h"
#include "sqlite_prepared_statement_handle.h"
#include "sqlite_select_statement.h"
#include "sqlite_types.h"
#include "sqlite_update_statement.h"

namespace stonks::sqlite {
namespace {
[[nodiscard]] auto Logger() -> spdlog::logger & {
  static auto logger = spdlog::stdout_color_mt("SqliteDb");
  return *logger;
}
}  // namespace

class SqliteDb::Impl {
 public:
  explicit Impl(SqliteDbHandle sqlite_db_handle)
      : sqlite_db_handle_{std::move(sqlite_db_handle)},
        sqlite_db_facade_{cpp::assume_not_null(sqlite_db_handle_.get())} {}

  Impl(const Impl &) = delete;
  Impl(Impl &&) noexcept = default;

  auto operator=(const Impl &) -> Impl & = delete;
  auto operator=(Impl &&) noexcept -> Impl & = default;

  ~Impl() noexcept = default;

  [[nodiscard]] auto PrepareStatement(
      std::string_view query, const sqldb::RowDefinition &result_definition)
      -> cpp::not_null<std::unique_ptr<sqldb::ISelectStatement>> {
    return cpp::assume_not_null(std::make_unique<SqliteSelectStatement>(
        SqlitePreparedStatementHandle{
            sqlite_db_handle_,
            sqlite_db_facade_.CreatePreparedStatement(query)},
        result_definition));
  }

  [[nodiscard]] auto PrepareStatement(std::string_view query)
      -> cpp::not_null<std::unique_ptr<sqldb::IUpdateStatement>> {
    return cpp::assume_not_null(
        std::make_unique<SqliteUpdateStatement>(SqlitePreparedStatementHandle{
            sqlite_db_handle_,
            sqlite_db_facade_.CreatePreparedStatement(query)}));
  }

  void WriteToFile(std::string_view file_path) {
    sqlite_db_facade_.WriteToFile(file_path);
  }

 private:
  cpp::not_null<std::shared_ptr<sqlite3>> sqlite_db_handle_;
  SqliteDbFacade sqlite_db_facade_;
};

SqliteDb::SqliteDb(SqliteDbHandle sqlite_db_handle)
    : impl_{cpp::assume_not_null(
          std::make_unique<Impl>(std::move(sqlite_db_handle)))} {}

SqliteDb::~SqliteDb() noexcept = default;

auto SqliteDb::PrepareStatement(std::string_view query,
                                const sqldb::RowDefinition &result_definition)
    -> cpp::not_null<std::unique_ptr<sqldb::ISelectStatement>> {
  return impl_->PrepareStatement(query, result_definition);
}

auto SqliteDb::PrepareStatement(std::string_view query)
    -> cpp::not_null<std::unique_ptr<sqldb::IUpdateStatement>> {
  return impl_->PrepareStatement(query);
}

void SqliteDb::WriteToFile(std::string_view file_path) {
  impl_->WriteToFile(file_path);
}
}  // namespace stonks::sqlite