#include <mdpdf/modules/render/render_coordinator.hpp>

#include <mdpdf/core/events/event_types.hpp>

namespace mdpdf::modules {

RenderCoordinator::RenderCoordinator(core::IPdfEngine& engine,
                                     core::IPageRenderer& renderer,
                                     core::EventBus& bus)
    : engine_(engine), renderer_(renderer), bus_(bus) {}

void RenderCoordinator::request_page(core::DocumentHandle doc,
                                     core::PageIndex page, double scale,
                                     double dpr) {
  core::RenderRequest request;
  request.document = doc;
  request.page = page;
  request.scale = scale;
  request.device_pixel_ratio = dpr;

  renderer_.render_async(request, [this, request](core::Result<core::PageBitmap> result) {
    if (!result.is_ok()) {
      return;
    }
    bus_.publish(core::events::PageRendered{request, result.value()});
  });
}

}  // namespace mdpdf::modules
