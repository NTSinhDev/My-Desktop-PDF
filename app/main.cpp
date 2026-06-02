#include <mdpdf/app/application_context.hpp>
#include <mdpdf/infrastructure/pdfium/pdfium_library.hpp>
#include <mdpdf/ui/views/main_window.hpp>

#include <QApplication>
#include <QStandardPaths>

#include <filesystem>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("My Desktop PDF");
  QApplication::setOrganizationName("MyDesktopPDF");

  mdpdf::app::ApplicationContext context;
  const auto data_dir = QStandardPaths::writableLocation(
      QStandardPaths::AppLocalDataLocation);
  context.initialize_storage(data_dir.toStdWString());

  mdpdf::ui::MainWindow window(context);
  window.show();

  mdpdf::infra::PdfiumLibrary::instance().ensure_initialized();

  if (argc > 1) {
    window.open_file(std::filesystem::path(argv[1]));
  }

  const int code = app.exec();

  mdpdf::infra::PdfiumLibrary::instance().shutdown();
  return code;
}
