#ifndef STONKS_DB_SQLITE_SQLITE_UTILS_H_
#define STONKS_DB_SQLITE_SQLITE_UTILS_H_

#include <sqlite3.h>

#include <gsl/pointers>
#include <string_view>

namespace stonks::db::sqlite::utils {
/**
 * @brief Read SQLite DB from file.
 */
[[nodiscard]] auto ReadSqliteDbFromFile(std::string_view file_path)
    -> gsl::not_null<sqlite3 *>;

/**
 * @brief Write SQLite DB to file.
 */
void WriteSqliteDbToFile(sqlite3 &sqlite_db, std::string_view file_path);
}  // namespace stonks::db::sqlite::utils

#endif  // STONKS_DB_SQLITE_SQLITE_UTILS_H_