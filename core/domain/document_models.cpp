#include <mdpdf/core/domain/document_models.hpp>

#include <functional>

namespace mdpdf::core {

DocumentId make_document_id(const std::filesystem::path& path) {
  const auto hash =
      std::hash<std::wstring>{}(path.wstring());
  return DocumentId{std::to_string(hash)};
}

}  // namespace mdpdf::core
