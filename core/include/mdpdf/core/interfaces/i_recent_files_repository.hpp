#pragma once

#include <mdpdf/core/domain/document_models.hpp>

#include <filesystem>
#include <vector>

namespace mdpdf::core {

class IRecentFilesRepository {
public:
  virtual ~IRecentFilesRepository() = default;
  virtual std::vector<RecentEntry> list(int max_count = 20) = 0;
  virtual void touch(const std::filesystem::path& path, int page_hint = 0) = 0;
  virtual void remove(const std::filesystem::path& path) = 0;
};

}  // namespace mdpdf::core
