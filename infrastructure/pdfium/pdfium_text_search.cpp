#include <mdpdf/infrastructure/pdfium/pdfium_text_search.hpp>

#include <fpdf_text.h>

namespace mdpdf::infra {

PdfiumTextSearch::PdfiumTextSearch(PdfiumEngine& engine, RenderThreadPool& pool)
    : engine_(engine), pool_(pool) {}

void PdfiumTextSearch::search_async(core::SearchQuery query,
                                  core::SearchProgressCallback on_progress,
                                  core::SearchCompleteCallback on_complete) {
  cancel_flag_.store(false);
  pool_.enqueue([this, query = std::move(query),
                 on_progress = std::move(on_progress),
                 on_complete = std::move(on_complete)]() mutable {
    std::vector<core::SearchHit> hits;
    auto* entry = engine_.entry(query.document);
    if (!entry) {
      on_complete(std::move(hits));
      return;
    }

    const int total = entry->info.page_count;
    for (int page_index = 0; page_index < total; ++page_index) {
      if (cancel_flag_.load()) {
        on_complete(std::move(hits));
        return;
      }
      if (on_progress) {
        on_progress(page_index, total);
      }

      std::scoped_lock lock(entry->mutex);
      FPDF_PAGE page = FPDF_LoadPage(entry->doc, page_index);
      if (!page) {
        continue;
      }
      FPDF_TEXTPAGE text_page = FPDFText_LoadPage(page);
      FPDF_ClosePage(page);

      if (!text_page) {
        continue;
      }

      const auto flags = query.case_sensitive ? FPDF_MATCHCASE : 0;
      FPDF_SCHHANDLE search =
          FPDFText_FindStart(text_page, query.text.c_str(), flags, 0);
      while (FPDFText_FindNext(search)) {
        const int char_index = FPDFText_GetSchResultIndex(search);
        const int char_count = FPDFText_GetSchCount(search);

        core::SearchHit hit;
        hit.page.value = page_index;
        hit.char_index = char_index;

        for (int i = 0; i < char_count; ++i) {
          double left = 0, right = 0, bottom = 0, top = 0;
          FPDFText_GetCharBox(text_page, char_index + i, &left, &right, &bottom,
                              &top);
          hit.rects.push_back(core::Rect{left, top, right, bottom});
        }
        hits.push_back(std::move(hit));
      }
      FPDFText_FindClose(search);
      FPDFText_ClosePage(text_page);
    }
    on_complete(std::move(hits));
  });
}

void PdfiumTextSearch::cancel() { cancel_flag_.store(true); }

}  // namespace mdpdf::infra
