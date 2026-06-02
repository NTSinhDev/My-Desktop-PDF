#pragma once

#include <mdpdf/core/events/event_bus.hpp>
#include <mdpdf/core/interfaces/i_page_renderer.hpp>
#include <mdpdf/core/interfaces/i_pdf_engine.hpp>

namespace mdpdf::modules {

class RenderCoordinator {
public:
  RenderCoordinator(core::IPdfEngine& engine, core::IPageRenderer& renderer,
                    core::EventBus& bus);

  void request_page(core::DocumentHandle doc, core::PageIndex page, double scale,
                    double dpr);

private:
  core::IPdfEngine& engine_;
  core::IPageRenderer& renderer_;
  core::EventBus& bus_;
};

}  // namespace mdpdf::modules
