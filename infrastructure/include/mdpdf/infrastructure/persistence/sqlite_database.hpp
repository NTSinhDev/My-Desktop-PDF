#pragma once

#include <filesystem>
#include <mutex>
#include <sqlite3.h>

namespace mdpdf::infra {

class SqliteDatabase {
public:
  static SqliteDatabase& instance();

  void open(const std::filesystem::path& path);
  sqlite3* handle();
  void exec(const char* sql);

private:
  SqliteDatabase() = default;
  std::mutex mutex_;
  sqlite3* db_ = nullptr;
};

}  // namespace mdpdf::infra
