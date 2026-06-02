#include <mdpdf/infrastructure/pdfium/pdfium_renderer.hpp>

#include <fpdfview.h>

#include <algorithm>

namespace mdpdf::infra {

namespace {
CacheKey make_key(const core::RenderRequest& req) {
  CacheKey key;
  key.document = req.document;
  key.page = req.page.value;
  key.scale_milli = static_cast<int>(req.scale * 1000.0);
  key.dpr_milli = static_cast<int>(req.device_pixel_ratio * 1000.0);
  return key;
}

constexpr int kMaxEdgePixels = 4096;
}  // namespace

PdfiumRenderer::PdfiumRenderer(PdfiumEngine& engine, LruPageCache& cache,
                               RenderThreadPool& pool)
    : engine_(engine), cache_(cache), pool_(pool) {}

void PdfiumRenderer::render_async(core::RenderRequest request,
                                  core::RenderCallback callback) {
  const auto key = make_key(request);
  if (auto cached = cache_.get(key)) {
    callback(core::Result<core::PageBitmap>::ok(*cached));
    return;
  }

  pool_.enqueue([this, request, key, callback = std::move(callback)]() mutable {
    if (cancelled_.load()) {
      return;
    }
    auto result = render_sync(request);
    if (result.is_ok()) {
      cache_.put(key, result.value());
    }
    callback(std::move(result));
  });
}

void PdfiumRenderer::cancel_pending(core::DocumentHandle doc) {
  cancelled_.store(true);
  cache_.invalidate_document(doc);
  cancelled_.store(false);
}

core::Result<core::PageBitmap> PdfiumRenderer::render_sync(
    const core::RenderRequest& request) {
  auto* entry = engine_.entry(request.document);
  if (!entry) {
    return core::Result<core::PageBitmap>::fail("Document not found");
  }

  std::scoped_lock lock(entry->mutex);
  FPDF_PAGE page = FPDF_LoadPage(entry->doc, request.page.value);
  if (!page) {
    return core::Result<core::PageBitmap>::fail("Failed to load page");
  }

  const double scale =
      request.scale * request.device_pixel_ratio;
  int width = static_cast<int>(FPDF_GetPageWidthF(page) * scale);
  int height = static_cast<int>(FPDF_GetPageHeightF(page) * scale);
  if (width > kMaxEdgePixels || height > kMaxEdgePixels) {
    const double factor =
        static_cast<double>(kMaxEdgePixels) /
        static_cast<double>(std::max(width, height));
    width = static_cast<int>(width * factor);
    height = static_cast<int>(height * factor);
  }
  width = std::max(width, 1);
  height = std::max(height, 1);

  FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 1);
  FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF);
  FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0,
                        FPDF_ANNOT | FPDF_LCD_TEXT);

  core::PageBitmap result;
  result.width = width;
  result.height = height;
  result.stride = FPDFBitmap_GetStride(bitmap);
  const auto buffer = static_cast<const std::uint8_t*>(FPDFBitmap_GetBuffer(bitmap));
  const std::size_t size =
      static_cast<std::size_t>(result.stride) * static_cast<std::size_t>(height);
  result.bgra.assign(buffer, buffer + size);

  FPDFBitmap_Destroy(bitmap);
  FPDF_ClosePage(page);
  return core::Result<core::PageBitmap>::ok(std::move(result));
}

}  // namespace mdpdf::infra
