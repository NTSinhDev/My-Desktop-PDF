#include <mdpdf/infrastructure/pdfium/pdfium_text_selection.hpp>

#include <fpdf_text.h>

#include <Windows.h>

#include <algorithm>
#include <vector>

namespace mdpdf::infra {

PdfiumTextSelection::PdfiumTextSelection(PdfiumEngine& engine) : engine_(engine) {}

core::Result<int> PdfiumTextSelection::char_index_at_point(core::DocumentHandle doc,
                                                           core::PageIndex page,
                                                           double x, double y,
                                                           double scale) {
  auto* entry = engine_.entry(doc);
  if (!entry) {
    return core::Result<int>::fail("Document not found");
  }

  std::scoped_lock lock(entry->mutex);
  FPDF_PAGE pdf_page = FPDF_LoadPage(entry->doc, page.value);
  if (!pdf_page) {
    return core::Result<int>::fail("Failed to load page");
  }
  FPDF_TEXTPAGE text_page = FPDFText_LoadPage(pdf_page);
  FPDF_ClosePage(pdf_page);
  if (!text_page) {
    return core::Result<int>::fail("Failed to load text page");
  }

  const int index = FPDFText_GetCharIndexAtPos(text_page, x / scale, y / scale,
                                               10.0 / scale, 10.0 / scale);
  FPDFText_ClosePage(text_page);
  if (index < 0) {
    return core::Result<int>::fail("No character at point");
  }
  return core::Result<int>::ok(index);
}

core::Result<core::TextSelection> PdfiumTextSelection::selection_range(
    core::DocumentHandle doc, core::PageIndex page, int start_char, int end_char) {
  auto* entry = engine_.entry(doc);
  if (!entry) {
    return core::Result<core::TextSelection>::fail("Document not found");
  }

  const int begin = std::min(start_char, end_char);
  const int end = std::max(start_char, end_char);
  if (begin == end) {
    return core::Result<core::TextSelection>::fail("Empty selection");
  }

  std::scoped_lock lock(entry->mutex);
  FPDF_PAGE pdf_page = FPDF_LoadPage(entry->doc, page.value);
  if (!pdf_page) {
    return core::Result<core::TextSelection>::fail("Failed to load page");
  }
  FPDF_TEXTPAGE text_page = FPDFText_LoadPage(pdf_page);
  FPDF_ClosePage(pdf_page);
  if (!text_page) {
    return core::Result<core::TextSelection>::fail("Failed to load text page");
  }

  const int count = end - begin;
  std::vector<unsigned short> buffer(static_cast<size_t>(count) * 2 + 2);
  const int written =
      FPDFText_GetText(text_page, begin, count, buffer.data());
  FPDFText_ClosePage(text_page);
  if (written <= 0) {
    return core::Result<core::TextSelection>::fail("Failed to extract text");
  }

  core::TextSelection selection;
  selection.page = page;
  selection.start_char = begin;
  selection.end_char = end;
  const wchar_t* utf16 = reinterpret_cast<const wchar_t*>(buffer.data());
  const int wchar_count = written > 0 ? written - 1 : 0;
  if (wchar_count > 0) {
    const int bytes = WideCharToMultiByte(CP_UTF8, 0, utf16, wchar_count, nullptr,
                                          0, nullptr, nullptr);
    if (bytes > 0) {
      selection.utf8_text.resize(static_cast<size_t>(bytes));
      WideCharToMultiByte(CP_UTF8, 0, utf16, wchar_count, selection.utf8_text.data(),
                          bytes, nullptr, nullptr);
    }
  }
  return core::Result<core::TextSelection>::ok(std::move(selection));
}

}  // namespace mdpdf::infra
