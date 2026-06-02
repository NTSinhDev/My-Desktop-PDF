#include <mdpdf/ui/utils/bitmap_utils.hpp>

#include <cstring>

namespace mdpdf::ui {

QImage to_qimage(const core::PageBitmap& bitmap) {
  if (bitmap.bgra.empty() || bitmap.width <= 0 || bitmap.height <= 0) {
    return {};
  }
  QImage image(bitmap.width, bitmap.height, QImage::Format_ARGB32);
  const int dst_stride = image.bytesPerLine();
  const auto* src = bitmap.bgra.data();
  auto* dst = image.bits();
  const int copy_width = std::min(bitmap.stride, dst_stride);
  for (int y = 0; y < bitmap.height; ++y) {
    std::memcpy(dst + y * dst_stride, src + y * bitmap.stride,
                static_cast<size_t>(copy_width));
  }
  return image.convertToFormat(QImage::Format_RGB32);
}

}  // namespace mdpdf::ui
