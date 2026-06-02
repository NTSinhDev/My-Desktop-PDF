#pragma once

#include <mdpdf/core/interfaces/i_text_search_service.hpp>
#include <mdpdf/infrastructure/pdfium/pdfium_engine.hpp>
#include <mdpdf/infrastructure/threading/render_thread_pool.hpp>

#include <atomic>

namespace mdpdf::infra {

class PdfiumTextSearch final : public core::ITextSearchService {
public:
  explicit PdfiumTextSearch(PdfiumEngine& engine, RenderThreadPool& pool);

  void search_async(core::SearchQuery query, core::SearchProgressCallback on_progress,
                    core::SearchCompleteCallback on_complete) override;
  void cancel() override;

private:
  PdfiumEngine& engine_;
  RenderThreadPool& pool_;
  std::atomic<bool> cancel_flag_{false};
};

}  // namespace mdpdf::infra
