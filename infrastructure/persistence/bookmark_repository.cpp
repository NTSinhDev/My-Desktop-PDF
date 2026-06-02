#include <mdpdf/infrastructure/persistence/bookmark_repository.hpp>

#include <mdpdf/infrastructure/persistence/sqlite_database.hpp>

namespace mdpdf::infra {

std::vector<core::Bookmark> SqliteBookmarkRepository::list_for_document(
    const std::filesystem::path& path) {
  std::vector<core::Bookmark> result;
  auto* db = SqliteDatabase::instance().handle();
  if (!db) {
    return result;
  }

  sqlite3_stmt* stmt = nullptr;
  const char* sql =
      "SELECT id, page, title, created_at FROM bookmarks WHERE document_path = "
      "? ORDER BY page";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    return result;
  }
  sqlite3_bind_text(stmt, 1, path.string().c_str(), -1, SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    core::Bookmark b;
    b.id = sqlite3_column_int64(stmt, 0);
    b.document_path = path;
    b.page.value = sqlite3_column_int(stmt, 1);
    b.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    const auto ts = sqlite3_column_int64(stmt, 3);
    b.created_at = std::chrono::system_clock::time_point{
        std::chrono::seconds(ts)};
    result.push_back(std::move(b));
  }
  sqlite3_finalize(stmt);
  return result;
}

core::Bookmark SqliteBookmarkRepository::add(const core::Bookmark& bookmark) {
  auto* db = SqliteDatabase::instance().handle();
  core::Bookmark result = bookmark;
  if (!db) {
    return result;
  }

  sqlite3_stmt* stmt = nullptr;
  const char* sql =
      "INSERT INTO bookmarks(document_path, page, title, created_at) VALUES(?, "
      "?, ?, ?)";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    return result;
  }
  const auto ts = std::chrono::duration_cast<std::chrono::seconds>(
                      bookmark.created_at.time_since_epoch())
                      .count();
  sqlite3_bind_text(stmt, 1, bookmark.document_path.string().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, bookmark.page.value);
  sqlite3_bind_text(stmt, 3, bookmark.title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int64(stmt, 4, ts);
  sqlite3_step(stmt);
  result.id = sqlite3_last_insert_rowid(db);
  sqlite3_finalize(stmt);
  return result;
}

void SqliteBookmarkRepository::remove(std::int64_t id) {
  auto* db = SqliteDatabase::instance().handle();
  if (!db) {
    return;
  }
  sqlite3_stmt* stmt = nullptr;
  const char* sql = "DELETE FROM bookmarks WHERE id = ?";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
    sqlite3_bind_int64(stmt, 1, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }
}

}  // namespace mdpdf::infra
