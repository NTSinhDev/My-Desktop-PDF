#pragma once

#include <mdpdf/core/domain/document_models.hpp>
#include <mdpdf/core/result.hpp>

#include <functional>

namespace mdpdf::core {

using RenderCallback = std::function<void(Result<PageBitmap>)>;

class IPageRenderer {
public:
  virtual ~IPageRenderer() = default;
  virtual void render_async(RenderRequest request, RenderCallback callback) = 0;
  virtual void cancel_pending(DocumentHandle doc) = 0;
};

}  // namespace mdpdf::core
