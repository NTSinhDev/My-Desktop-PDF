#include <mdpdf/infrastructure/persistence/recent_files_repository.hpp>

#include <mdpdf/infrastructure/persistence/sqlite_database.hpp>

namespace mdpdf::infra {

std::vector<core::RecentEntry> SqliteRecentFilesRepository::list(int max_count) {
  std::vector<core::RecentEntry> result;
  auto* db = SqliteDatabase::instance().handle();
  if (!db) {
    return result;
  }

  sqlite3_stmt* stmt = nullptr;
  const char* sql =
      "SELECT path, last_opened_at, page_hint FROM recent_files ORDER BY "
      "last_opened_at DESC LIMIT ?";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    return result;
  }
  sqlite3_bind_int(stmt, 1, max_count);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    core::RecentEntry entry;
    entry.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    const auto ts = sqlite3_column_int64(stmt, 1);
    entry.last_opened_at = std::chrono::system_clock::time_point{
        std::chrono::seconds(ts)};
    entry.page_hint = sqlite3_column_int(stmt, 2);
    result.push_back(std::move(entry));
  }
  sqlite3_finalize(stmt);
  return result;
}

void SqliteRecentFilesRepository::touch(const std::filesystem::path& path,
                                        int page_hint) {
  auto* db = SqliteDatabase::instance().handle();
  if (!db) {
    return;
  }
  const auto ts = std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();

  sqlite3_stmt* stmt = nullptr;
  const char* sql =
      "INSERT INTO recent_files(path, last_opened_at, page_hint) VALUES(?, ?, ?) "
      "ON CONFLICT(path) DO UPDATE SET last_opened_at=excluded.last_opened_at, "
      "page_hint=excluded.page_hint";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
    sqlite3_bind_text(stmt, 1, path.string().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, ts);
    sqlite3_bind_int(stmt, 3, page_hint);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }
}

void SqliteRecentFilesRepository::remove(const std::filesystem::path& path) {
  auto* db = SqliteDatabase::instance().handle();
  if (!db) {
    return;
  }
  sqlite3_stmt* stmt = nullptr;
  const char* sql = "DELETE FROM recent_files WHERE path = ?";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
    sqlite3_bind_text(stmt, 1, path.string().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }
}

}  // namespace mdpdf::infra
