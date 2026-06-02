#pragma once

#include <mdpdf/core/domain/document_models.hpp>

#include <vector>

namespace mdpdf::core::events {

struct DocumentOpened {
  PdfDocumentInfo info;
};

struct DocumentClosed {
  DocumentHandle handle = kInvalidDocumentHandle;
};

struct PageRendered {
  RenderRequest request;
  PageBitmap bitmap;
};

struct SearchProgress {
  int current_page = 0;
  int total_pages = 0;
};

struct SearchCompleted {
  std::vector<SearchHit> hits;
  bool cancelled = false;
};

struct ZoomChanged {
  double scale = 1.0;
};

}  // namespace mdpdf::core::events
