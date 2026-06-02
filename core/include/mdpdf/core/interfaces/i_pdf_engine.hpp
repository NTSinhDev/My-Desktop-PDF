#pragma once

#include <mdpdf/core/domain/document_models.hpp>
#include <mdpdf/core/result.hpp>

#include <filesystem>

namespace mdpdf::core {

class IPdfEngine {
public:
  virtual ~IPdfEngine() = default;
  virtual Result<PdfDocumentInfo> open(const std::filesystem::path& path) = 0;
  virtual void close(DocumentHandle handle) = 0;
  virtual Result<PageLayout> page_layout(DocumentHandle doc, PageIndex page) = 0;
};

}  // namespace mdpdf::core
