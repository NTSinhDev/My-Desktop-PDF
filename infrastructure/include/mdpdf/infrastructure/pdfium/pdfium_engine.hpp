#pragma once

#include <mdpdf/core/interfaces/i_pdf_engine.hpp>

#include <fpdfview.h>
#include <mutex>
#include <unordered_map>

namespace mdpdf::infra {

struct PdfiumDocumentEntry {
  FPDF_DOCUMENT doc = nullptr;
  core::PdfDocumentInfo info;
  std::mutex mutex;
};

class PdfiumEngine final : public core::IPdfEngine {
public:
  core::Result<core::PdfDocumentInfo> open(
      const std::filesystem::path& path) override;
  void close(core::DocumentHandle handle) override;
  core::Result<core::PageLayout> page_layout(core::DocumentHandle doc,
                                             core::PageIndex page) override;

  PdfiumDocumentEntry* entry(core::DocumentHandle handle);

private:
  std::mutex map_mutex_;
  std::unordered_map<core::DocumentHandle, PdfiumDocumentEntry> documents_;
  core::DocumentHandle next_handle_ = 1;
};

}  // namespace mdpdf::infra
