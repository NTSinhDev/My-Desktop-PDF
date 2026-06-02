#pragma once

#include <mdpdf/core/domain/document_models.hpp>

#include <filesystem>
#include <vector>

namespace mdpdf::core {

class IBookmarkRepository {
public:
  virtual ~IBookmarkRepository() = default;
  virtual std::vector<Bookmark> list_for_document(
      const std::filesystem::path& path) = 0;
  virtual Bookmark add(const Bookmark& bookmark) = 0;
  virtual void remove(std::int64_t id) = 0;
};

}  // namespace mdpdf::core
