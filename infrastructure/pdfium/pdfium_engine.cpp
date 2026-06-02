#include <mdpdf/infrastructure/pdfium/pdfium_engine.hpp>

#include <mdpdf/infrastructure/pdfium/pdfium_library.hpp>

namespace mdpdf::infra {

core::Result<core::PdfDocumentInfo> PdfiumEngine::open(
    const std::filesystem::path& path) {
  PdfiumLibrary::instance().ensure_initialized();

  const auto native = path.native();
  FPDF_DOCUMENT doc = FPDF_LoadDocument(native.c_str(), nullptr);
  if (!doc) {
    const auto err = FPDF_GetLastError();
    return core::Result<core::PdfDocumentInfo>::fail(
        "Failed to open PDF (error " + std::to_string(err) + ")");
  }

  core::PdfDocumentInfo info;
  info.handle = next_handle_++;
  info.id = core::make_document_id(path);
  info.path = path;
  info.page_count = static_cast<int>(FPDF_GetPageCount(doc));
  info.encrypted = FPDF_GetSecurityHandlerRevision(doc) != 0;

  std::wstring title;
  title.resize(512);
  const auto len = FPDF_GetMetaText(doc, "Title", title.data(), 512);
  if (len > 2) {
    title.resize(static_cast<size_t>(len / 2 - 1));
    info.title.assign(title.begin(), title.end());
  }

  PdfiumDocumentEntry entry;
  entry.doc = doc;
  entry.info = info;

  std::scoped_lock lock(map_mutex_);
  documents_.emplace(info.handle, std::move(entry));
  return core::Result<core::PdfDocumentInfo>::ok(info);
}

void PdfiumEngine::close(core::DocumentHandle handle) {
  std::scoped_lock lock(map_mutex_);
  const auto it = documents_.find(handle);
  if (it == documents_.end()) {
    return;
  }
  if (it->second.doc) {
    FPDF_CloseDocument(it->second.doc);
  }
  documents_.erase(it);
}

core::Result<core::PageLayout> PdfiumEngine::page_layout(core::DocumentHandle doc,
                                                         core::PageIndex page) {
  auto* entry = this->entry(doc);
  if (!entry) {
    return core::Result<core::PageLayout>::fail("Document not found");
  }
  std::scoped_lock lock(entry->mutex);
  FPDF_PAGE pdf_page = FPDF_LoadPage(entry->doc, page.value);
  if (!pdf_page) {
    return core::Result<core::PageLayout>::fail("Failed to load page");
  }
  core::PageLayout layout;
  layout.width_points = FPDF_GetPageWidthF(pdf_page);
  layout.height_points = FPDF_GetPageHeightF(pdf_page);
  layout.rotation = FPDFPage_GetRotation(pdf_page);
  FPDF_ClosePage(pdf_page);
  return core::Result<core::PageLayout>::ok(layout);
}

PdfiumDocumentEntry* PdfiumEngine::entry(core::DocumentHandle handle) {
  std::scoped_lock lock(map_mutex_);
  const auto it = documents_.find(handle);
  if (it == documents_.end()) {
    return nullptr;
  }
  return &it->second;
}

}  // namespace mdpdf::infra
