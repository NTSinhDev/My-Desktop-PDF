#pragma once

#include <mdpdf/core/interfaces/i_page_renderer.hpp>
#include <mdpdf/infrastructure/cache/lru_page_cache.hpp>
#include <mdpdf/infrastructure/pdfium/pdfium_engine.hpp>
#include <mdpdf/infrastructure/threading/render_thread_pool.hpp>

#include <atomic>

namespace mdpdf::infra {

class PdfiumRenderer final : public core::IPageRenderer {
public:
  PdfiumRenderer(PdfiumEngine& engine, LruPageCache& cache, RenderThreadPool& pool);

  void render_async(core::RenderRequest request,
                    core::RenderCallback callback) override;
  void cancel_pending(core::DocumentHandle doc) override;

private:
  core::Result<core::PageBitmap> render_sync(const core::RenderRequest& request);

  PdfiumEngine& engine_;
  LruPageCache& cache_;
  RenderThreadPool& pool_;
  std::atomic<bool> cancelled_{false};
};

}  // namespace mdpdf::infra
