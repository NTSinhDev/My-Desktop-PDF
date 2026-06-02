#include <mdpdf/app/application_context.hpp>

#include <mdpdf/infrastructure/logging/logger.hpp>
#include <mdpdf/infrastructure/persistence/sqlite_database.hpp>

namespace mdpdf::app {

ApplicationContext::ApplicationContext()
    : page_cache_(48, 256 * 1024 * 1024) {
  pdf_engine_ = std::make_unique<infra::PdfiumEngine>();
  pdf_renderer_ = std::make_unique<infra::PdfiumRenderer>(
      *pdf_engine_, page_cache_, thread_pool_);
  text_search_ =
      std::make_unique<infra::PdfiumTextSearch>(*pdf_engine_, thread_pool_);
  text_selection_ = std::make_unique<infra::PdfiumTextSelection>(*pdf_engine_);
  bookmark_repo_ = std::make_unique<infra::SqliteBookmarkRepository>();
  recent_repo_ = std::make_unique<infra::SqliteRecentFilesRepository>();
  document_service_ = std::make_unique<modules::DocumentService>(
      *pdf_engine_, *recent_repo_, event_bus_);
  render_coordinator_ = std::make_unique<modules::RenderCoordinator>(
      *pdf_engine_, *pdf_renderer_, event_bus_);
}

void ApplicationContext::initialize_storage(
    const std::filesystem::path& app_data_dir) {
  std::filesystem::create_directories(app_data_dir);
  init_logging(app_data_dir / "mydesktoppdf.log");
  infra::SqliteDatabase::instance().open(app_data_dir / "app.db");
  MDPDF_LOG_INFO("Storage initialized at {}", app_data_dir.string());
}

}  // namespace mdpdf::app
