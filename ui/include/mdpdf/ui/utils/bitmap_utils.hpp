#pragma once

#include <mdpdf/core/domain/document_models.hpp>

#include <QImage>

namespace mdpdf::ui {

QImage to_qimage(const core::PageBitmap& bitmap);

}  // namespace mdpdf::ui
