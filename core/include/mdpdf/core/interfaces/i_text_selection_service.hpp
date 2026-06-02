#pragma once

#include <mdpdf/core/domain/document_models.hpp>
#include <mdpdf/core/result.hpp>

namespace mdpdf::core {

class ITextSelectionService {
public:
  virtual ~ITextSelectionService() = default;
  virtual Result<int> char_index_at_point(DocumentHandle doc, PageIndex page,
                                          double x, double y, double scale) = 0;
  virtual Result<TextSelection> selection_range(DocumentHandle doc, PageIndex page,
                                               int start_char, int end_char) = 0;
};

}  // namespace mdpdf::core
