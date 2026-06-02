#pragma once

#include <mdpdf/core/interfaces/i_bookmark_repository.hpp>

namespace mdpdf::infra {

class SqliteBookmarkRepository final : public core::IBookmarkRepository {
public:
  std::vector<core::Bookmark> list_for_document(
      const std::filesystem::path& path) override;
  core::Bookmark add(const core::Bookmark& bookmark) override;
  void remove(std::int64_t id) override;
};

}  // namespace mdpdf::infra
