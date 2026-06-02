#pragma once

#include <mdpdf/core/interfaces/i_recent_files_repository.hpp>

namespace mdpdf::infra {

class SqliteRecentFilesRepository final : public core::IRecentFilesRepository {
public:
  std::vector<core::RecentEntry> list(int max_count = 20) override;
  void touch(const std::filesystem::path& path, int page_hint = 0) override;
  void remove(const std::filesystem::path& path) override;
};

}  // namespace mdpdf::infra
