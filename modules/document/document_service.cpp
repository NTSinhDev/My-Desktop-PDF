#include <mdpdf/modules/document/document_service.hpp>

#include <mdpdf/core/events/event_types.hpp>

namespace mdpdf::modules {

DocumentService::DocumentService(core::IPdfEngine& engine,
                                 core::IRecentFilesRepository& recent,
                                 core::EventBus& bus)
    : engine_(engine), recent_(recent), bus_(bus) {}

core::Result<core::PdfDocumentInfo> DocumentService::open(
    const std::filesystem::path& path) {
  if (current_) {
    close();
  }
  auto result = engine_.open(path);
  if (!result.is_ok()) {
    return result;
  }
  current_ = std::make_unique<core::PdfDocumentInfo>(result.value());
  recent_.touch(path, 0);
  bus_.publish(core::events::DocumentOpened{*current_});
  return result;
}

void DocumentService::close() {
  if (!current_) {
    return;
  }
  const auto handle = current_->handle;
  engine_.close(handle);
  bus_.publish(core::events::DocumentClosed{handle});
  current_.reset();
}

}  // namespace mdpdf::modules
