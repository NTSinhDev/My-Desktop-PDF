#pragma once

#include <mdpdf/core/interfaces/i_text_selection_service.hpp>
#include <mdpdf/infrastructure/pdfium/pdfium_engine.hpp>

namespace mdpdf::infra {

class PdfiumTextSelection final : public core::ITextSelectionService {
public:
  explicit PdfiumTextSelection(PdfiumEngine& engine);

  core::Result<int> char_index_at_point(core::DocumentHandle doc,
                                        core::PageIndex page, double x, double y,
                                        double scale) override;
  core::Result<core::TextSelection> selection_range(core::DocumentHandle doc,
                                                   core::PageIndex page,
                                                   int start_char,
                                                   int end_char) override;

private:
  PdfiumEngine& engine_;
};

}  // namespace mdpdf::infra
