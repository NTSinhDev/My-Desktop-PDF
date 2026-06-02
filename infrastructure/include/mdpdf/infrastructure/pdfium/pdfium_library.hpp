#pragma once

namespace mdpdf::infra {

class PdfiumLibrary {
public:
  static PdfiumLibrary& instance();
  void ensure_initialized();
  void shutdown();

private:
  PdfiumLibrary() = default;
  bool initialized_ = false;
};

}  // namespace mdpdf::infra
