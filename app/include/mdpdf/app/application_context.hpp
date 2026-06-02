#pragma once

#include <mdpdf/core/events/event_bus.hpp>
#include <mdpdf/infrastructure/cache/lru_page_cache.hpp>
#include <mdpdf/infrastructure/pdfium/pdfium_engine.hpp>
#include <mdpdf/infrastructure/pdfium/pdfium_renderer.hpp>
#include <mdpdf/infrastructure/pdfium/pdfium_text_search.hpp>
#include <mdpdf/infrastructure/pdfium/pdfium_text_selection.hpp>
#include <mdpdf/infrastructure/persistence/bookmark_repository.hpp>
#include <mdpdf/infrastructure/persistence/recent_files_repository.hpp>
#include <mdpdf/infrastructure/threading/render_thread_pool.hpp>
#include <mdpdf/modules/document/document_service.hpp>
#include <mdpdf/modules/render/render_coordinator.hpp>

#include <filesystem>
#include <memory>

namespace mdpdf::app {

class ApplicationContext {
public:
  ApplicationContext();

  void initialize_storage(const std::filesystem::path& app_data_dir);

  core::EventBus& event_bus() { return event_bus_; }
  modules::DocumentService& documents() { return *document_service_; }
  modules::RenderCoordinator& render() { return *render_coordinator_; }
  infra::PdfiumEngine& engine() { return *pdf_engine_; }
  infra::PdfiumRenderer& renderer() { return *pdf_renderer_; }
  infra::PdfiumTextSearch& search() { return *text_search_; }
  infra::PdfiumTextSelection& selection() { return *text_selection_; }
  infra::SqliteBookmarkRepository& bookmarks() { return *bookmark_repo_; }
  infra::SqliteRecentFilesRepository& recent() { return *recent_repo_; }

private:
  core::EventBus event_bus_;
  infra::RenderThreadPool thread_pool_;
  infra::LruPageCache page_cache_;
  std::unique_ptr<infra::PdfiumEngine> pdf_engine_;
  std::unique_ptr<infra::PdfiumRenderer> pdf_renderer_;
  std::unique_ptr<infra::PdfiumTextSearch> text_search_;
  std::unique_ptr<infra::PdfiumTextSelection> text_selection_;
  std::unique_ptr<infra::SqliteBookmarkRepository> bookmark_repo_;
  std::unique_ptr<infra::SqliteRecentFilesRepository> recent_repo_;
  std::unique_ptr<modules::DocumentService> document_service_;
  std::unique_ptr<modules::RenderCoordinator> render_coordinator_;
};

}  // namespace mdpdf::app
