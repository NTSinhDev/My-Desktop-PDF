#pragma once

#include <mdpdf/app/application_context.hpp>

#include <mdpdf/core/events/event_types.hpp>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>

class QLabel;

namespace mdpdf::ui {

class PdfScrollView;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(mdpdf::app::ApplicationContext& context,
                      QWidget* parent = nullptr);

  void open_file(const std::filesystem::path& path);

private slots:
  void on_open();
  void on_zoom_in();
  void on_zoom_out();
  void on_zoom_reset();
  void on_search();
  void on_find_next();
  void on_find_previous();
  void on_copy();
  void on_add_bookmark();
  void on_recent_triggered(QListWidgetItem* item);
  void on_visible_range_changed(int first, int last);
  void on_page_rendered(const core::events::PageRendered& event);
  void on_document_opened(const core::events::DocumentOpened& event);
  void on_selection_changed(const core::TextSelection& selection);
  void on_scroll_page_clicked(int page);

private:
  void setup_ui();
  void setup_shortcuts();
  void refresh_recent_menu();
  void refresh_bookmarks();
  void request_visible_pages();
  void navigate_hit(int direction);

  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;

  mdpdf::app::ApplicationContext& context_;
  PdfScrollView* scroll_view_ = nullptr;
  QLineEdit* search_edit_ = nullptr;
  QLabel* status_label_ = nullptr;
  QListWidget* bookmark_list_ = nullptr;
  QMenu* recent_menu_ = nullptr;

  std::vector<core::SearchHit> search_hits_;
  int current_hit_index_ = -1;
  core::TextSelection last_selection_;
};

}  // namespace mdpdf::ui
