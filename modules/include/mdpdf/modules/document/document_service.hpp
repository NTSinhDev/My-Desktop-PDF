#pragma once

#include <memory>

#include <mdpdf/core/events/event_bus.hpp>
#include <mdpdf/core/interfaces/i_pdf_engine.hpp>
#include <mdpdf/core/interfaces/i_recent_files_repository.hpp>

namespace mdpdf::modules {

class DocumentService {
public:
  DocumentService(core::IPdfEngine& engine, core::IRecentFilesRepository& recent,
                  core::EventBus& bus);

  core::Result<core::PdfDocumentInfo> open(const std::filesystem::path& path);
  void close();

  const core::PdfDocumentInfo* current() const { return current_.get(); }

private:
  core::IPdfEngine& engine_;
  core::IRecentFilesRepository& recent_;
  core::EventBus& bus_;
  std::unique_ptr<core::PdfDocumentInfo> current_;
};

}  // namespace mdpdf::modules
