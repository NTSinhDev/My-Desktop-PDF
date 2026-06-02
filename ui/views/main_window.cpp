#include <mdpdf/ui/views/main_window.hpp>

#include <mdpdf/core/events/event_types.hpp>
#include <mdpdf/ui/utils/bitmap_utils.hpp>
#include <mdpdf/ui/views/pdf_scroll_view.hpp>

#include <QAction>
#include <QShortcut>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>

namespace mdpdf::ui {

MainWindow::MainWindow(mdpdf::app::ApplicationContext& context, QWidget* parent)
    : QMainWindow(parent), context_(context) {
  setup_ui();
  setup_shortcuts();

  context_.event_bus().subscribe<core::events::PageRendered>(
      [this](const core::events::PageRendered& e) { on_page_rendered(e); });
  context_.event_bus().subscribe<core::events::DocumentOpened>(
      [this](const core::events::DocumentOpened& e) { on_document_opened(e); });

  connect(scroll_view_, &PdfScrollView::visible_range_changed, this,
          &MainWindow::on_visible_range_changed);
  connect(scroll_view_, &PdfScrollView::selection_changed, this,
          &MainWindow::on_selection_changed);
  connect(scroll_view_, &PdfScrollView::selection_requested, this,
          [this](int page, double x, double y) {
            const auto doc = context_.documents().current();
            if (!doc) {
              return;
            }
            auto start = context_.selection().char_index_at_point(
                doc->handle, core::PageIndex{page}, x, y, scroll_view_->scale());
            if (!start.is_ok()) {
              return;
            }
            auto end = context_.selection().char_index_at_point(
                doc->handle, core::PageIndex{page}, x + 20, y, scroll_view_->scale());
            const int end_idx = end.is_ok() ? end.value() : start.value() + 1;
            auto sel = context_.selection().selection_range(
                doc->handle, core::PageIndex{page}, start.value(), end_idx);
            if (sel.is_ok()) {
              last_selection_ = sel.value();
              on_selection_changed(last_selection_);
            }
          });
  connect(scroll_view_, &PdfScrollView::page_clicked, this,
          &MainWindow::on_scroll_page_clicked);
  connect(scroll_view_, &PdfScrollView::zoom_changed, this, [this](double scale) {
    status_label_->setText(QString("Zoom: %1%").arg(static_cast<int>(scale * 100)));
  });

  refresh_recent_menu();
}

void MainWindow::setup_ui() {
  setWindowTitle("My Desktop PDF");
  resize(1200, 800);

  auto* file_menu = menuBar()->addMenu(tr("&File"));
  auto* open_action = file_menu->addAction(tr("&Open..."));
  connect(open_action, &QAction::triggered, this, &MainWindow::on_open);

  recent_menu_ = file_menu->addMenu(tr("Recent Files"));
  file_menu->addSeparator();
  file_menu->addAction(tr("E&xit"), qApp, &QApplication::quit);

  auto* view_menu = menuBar()->addMenu(tr("&View"));
  view_menu->addAction(tr("Zoom &In"), this, &MainWindow::on_zoom_in);
  view_menu->addAction(tr("Zoom &Out"), this, &MainWindow::on_zoom_out);
  view_menu->addAction(tr("&Reset Zoom"), this, &MainWindow::on_zoom_reset);

  auto* search_menu = menuBar()->addMenu(tr("&Search"));
  search_menu->addAction(tr("&Find..."), this, &MainWindow::on_search);
  search_menu->addAction(tr("Find &Next"), this, &MainWindow::on_find_next);
  search_menu->addAction(tr("Find Pre&vious"), this, &MainWindow::on_find_previous);

  auto* bookmark_menu = menuBar()->addMenu(tr("&Bookmarks"));
  bookmark_menu->addAction(tr("&Add Bookmark"), this, &MainWindow::on_add_bookmark);

  auto* toolbar = addToolBar(tr("Main"));
  toolbar->addAction(open_action);
  toolbar->addAction(tr("Copy"), this, &MainWindow::on_copy);

  search_edit_ = new QLineEdit(this);
  search_edit_->setPlaceholderText(tr("Search text..."));
  toolbar->addWidget(search_edit_);
  toolbar->addAction(tr("Find"), this, &MainWindow::on_search);

  scroll_view_ = new PdfScrollView(this);
  bookmark_list_ = new QListWidget(this);
  bookmark_list_->setMaximumWidth(240);

  auto* splitter = new QSplitter(this);
  splitter->addWidget(scroll_view_);
  splitter->addWidget(bookmark_list_);
  splitter->setStretchFactor(0, 1);
  setCentralWidget(splitter);

  status_label_ = new QLabel(tr("Ready"), this);
  statusBar()->addWidget(status_label_);

  setAcceptDrops(true);
}

void MainWindow::setup_shortcuts() {
  auto* open_sc = new QShortcut(QKeySequence::Open, this);
  connect(open_sc, &QShortcut::activated, this, &MainWindow::on_open);

  new QShortcut(QKeySequence::ZoomIn, this, SLOT(on_zoom_in()));
  new QShortcut(QKeySequence::ZoomOut, this, SLOT(on_zoom_out()));
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_0), this, SLOT(on_zoom_reset()));
  new QShortcut(QKeySequence::Find, this, SLOT(on_search()));
  new QShortcut(QKeySequence(Qt::Key_F3), this, SLOT(on_find_next()));
  new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F3), this, SLOT(on_find_previous()));
  new QShortcut(QKeySequence::Copy, this, SLOT(on_copy()));
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_B), this, SLOT(on_add_bookmark()));
}

void MainWindow::open_file(const std::filesystem::path& path) {
  auto result = context_.documents().open(path);
  if (!result.is_ok()) {
    QMessageBox::warning(
        this, tr("Open failed"),
        QString::fromStdString(result.error().message));
    return;
  }
  setWindowTitle(QString("My Desktop PDF - %1")
                     .arg(QString::fromStdWString(path.filename().wstring())));
  refresh_recent_menu();
  refresh_bookmarks();
}

void MainWindow::on_open() {
  const QString file = QFileDialog::getOpenFileName(
      this, tr("Open PDF"), {}, tr("PDF Files (*.pdf)"));
  if (!file.isEmpty()) {
    open_file(file.toStdWString());
  }
}

void MainWindow::on_zoom_in() {
  scroll_view_->set_scale(scroll_view_->scale() * 1.15);
  request_visible_pages();
}

void MainWindow::on_zoom_out() {
  scroll_view_->set_scale(scroll_view_->scale() / 1.15);
  request_visible_pages();
}

void MainWindow::on_zoom_reset() {
  scroll_view_->set_scale(1.0);
  request_visible_pages();
}

void MainWindow::on_search() {
  const auto doc = context_.documents().current();
  if (!doc) {
    return;
  }
  core::SearchQuery query;
  query.document = doc->handle;
  query.text = search_edit_->text().toStdString();
  status_label_->setText(tr("Searching..."));

  context_.search().search_async(
      query,
      [this](int page, int total) {
        QMetaObject::invokeMethod(
            this, [this, page, total]() {
              status_label_->setText(
                  QString("Searching page %1 / %2").arg(page + 1).arg(total));
            },
            Qt::QueuedConnection);
      },
      [this](std::vector<core::SearchHit> hits) {
        QMetaObject::invokeMethod(
            this,
            [this, hits = std::move(hits)]() mutable {
              search_hits_ = std::move(hits);
              current_hit_index_ = search_hits_.empty() ? -1 : 0;
              scroll_view_->set_search_highlights(search_hits_);
              status_label_->setText(
                  QString("Found %1 matches").arg(search_hits_.size()));
            },
            Qt::QueuedConnection);
      });
}

void MainWindow::on_find_next() { navigate_hit(1); }

void MainWindow::on_find_previous() { navigate_hit(-1); }

void MainWindow::navigate_hit(int direction) {
  if (search_hits_.empty()) {
    return;
  }
  current_hit_index_ =
      (current_hit_index_ + direction + static_cast<int>(search_hits_.size())) %
      static_cast<int>(search_hits_.size());
  scroll_view_->set_active_hit(current_hit_index_);
}

void MainWindow::on_copy() {
  if (last_selection_.utf8_text.empty()) {
    return;
  }
  QApplication::clipboard()->setText(
      QString::fromStdString(last_selection_.utf8_text));
}

void MainWindow::on_add_bookmark() {
  const auto doc = context_.documents().current();
  if (!doc) {
    return;
  }
  bool ok = false;
  QString title = QInputDialog::getText(
      this, tr("Bookmark"), tr("Title:"), QLineEdit::Normal,
      tr("Page %1").arg(1), &ok);
  if (!ok || title.isEmpty()) {
    return;
  }
  core::Bookmark bookmark;
  bookmark.document_path = doc->path;
  bookmark.page.value = 0;
  bookmark.title = title.toStdString();
  bookmark.created_at = std::chrono::system_clock::now();
  context_.bookmarks().add(bookmark);
  refresh_bookmarks();
}

void MainWindow::on_recent_triggered(QListWidgetItem* item) {
  if (!item) {
    return;
  }
  open_file(item->data(Qt::UserRole).toString().toStdWString());
}

void MainWindow::on_visible_range_changed(int first, int last) {
  Q_UNUSED(first);
  Q_UNUSED(last);
  request_visible_pages();
}

void MainWindow::request_visible_pages() {
  const auto doc = context_.documents().current();
  if (!doc) {
    return;
  }
  const auto [first, last] = scroll_view_->visible_page_range();
  const double scale = scroll_view_->scale();
  const double dpr = devicePixelRatioF();

  for (int page = first; page <= last; ++page) {
    const auto layout = context_.engine().page_layout(
        doc->handle, core::PageIndex{page});
    if (layout.is_ok()) {
      scroll_view_->set_page_layout(core::PageIndex{page}, layout.value());
    }
    context_.render().request_page(doc->handle, core::PageIndex{page}, scale,
                                   dpr);
  }
}

void MainWindow::on_page_rendered(const core::events::PageRendered& event) {
  QMetaObject::invokeMethod(
      this,
      [this, event]() {
        scroll_view_->set_page_image(event.request.page,
                                     to_qimage(event.bitmap));
      },
      Qt::QueuedConnection);
}

void MainWindow::on_document_opened(const core::events::DocumentOpened& event) {
  QMetaObject::invokeMethod(
      this,
      [this, info = event.info]() {
        scroll_view_->set_document(info.handle, info.page_count);
        request_visible_pages();
      },
      Qt::QueuedConnection);
}

void MainWindow::on_selection_changed(const core::TextSelection& selection) {
  const auto doc = context_.documents().current();
  if (!doc) {
    return;
  }
  auto result = context_.selection().selection_range(
      doc->handle, selection.page, selection.start_char, selection.end_char);
  if (result.is_ok()) {
    last_selection_ = result.value();
  }
}

void MainWindow::on_scroll_page_clicked(int page) {
  const auto doc = context_.documents().current();
  if (!doc) {
    return;
  }
  Q_UNUSED(page);
}

void MainWindow::refresh_recent_menu() {
  recent_menu_->clear();
  for (const auto& entry : context_.recent().list(15)) {
    auto* action = recent_menu_->addAction(
        QString::fromStdWString(entry.path.filename().wstring()));
    connect(action, &QAction::triggered, this, [this, entry]() {
      open_file(entry.path);
    });
  }
}

void MainWindow::refresh_bookmarks() {
  bookmark_list_->clear();
  const auto doc = context_.documents().current();
  if (!doc) {
    return;
  }
  for (const auto& b : context_.bookmarks().list_for_document(doc->path)) {
    auto* item = new QListWidgetItem(
        QString::fromStdString(b.title + " (p." + std::to_string(b.page.value + 1) + ")"));
    item->setData(Qt::UserRole, b.page.value);
    bookmark_list_->addItem(item);
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void MainWindow::dropEvent(QDropEvent* event) {
  const auto urls = event->mimeData()->urls();
  if (!urls.isEmpty()) {
    open_file(urls.first().toLocalFile().toStdWString());
  }
}

}  // namespace mdpdf::ui
