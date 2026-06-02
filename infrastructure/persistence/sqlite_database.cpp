#include <mdpdf/infrastructure/persistence/sqlite_database.hpp>

#include <mdpdf/infrastructure/logging/logger.hpp>

namespace mdpdf::infra {

namespace {
constexpr const char* kMigrationSql = R"SQL(
CREATE TABLE IF NOT EXISTS recent_files (
  path TEXT PRIMARY KEY,
  last_opened_at INTEGER NOT NULL,
  page_hint INTEGER NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS bookmarks (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  document_path TEXT NOT NULL,
  page INTEGER NOT NULL,
  title TEXT NOT NULL,
  created_at INTEGER NOT NULL
);
)SQL";
}  // namespace

SqliteDatabase& SqliteDatabase::instance() {
  static SqliteDatabase db;
  return db;
}

void SqliteDatabase::open(const std::filesystem::path& path) {
  std::scoped_lock lock(mutex_);
  if (db_) {
    return;
  }
  std::filesystem::create_directories(path.parent_path());
  if (sqlite3_open(path.string().c_str(), &db_) != SQLITE_OK) {
    MDPDF_LOG_ERROR("Failed to open database: {}", path.string());
    return;
  }
  exec(kMigrationSql);
}

sqlite3* SqliteDatabase::handle() {
  std::scoped_lock lock(mutex_);
  return db_;
}

void SqliteDatabase::exec(const char* sql) {
  char* err = nullptr;
  if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
    MDPDF_LOG_ERROR("SQLite error: {}", err ? err : "unknown");
    sqlite3_free(err);
  }
}

}  // namespace mdpdf::infra
