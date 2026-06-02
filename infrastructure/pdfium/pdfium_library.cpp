#include <mdpdf/infrastructure/pdfium/pdfium_library.hpp>

#include <fpdfview.h>

namespace mdpdf::infra {

PdfiumLibrary& PdfiumLibrary::instance() {
  static PdfiumLibrary lib;
  return lib;
}

void PdfiumLibrary::ensure_initialized() {
  if (initialized_) {
    return;
  }
  FPDF_InitLibrary();
  initialized_ = true;
}

void PdfiumLibrary::shutdown() {
  if (!initialized_) {
    return;
  }
  FPDF_DestroyLibrary();
  initialized_ = false;
}

}  // namespace mdpdf::infra
