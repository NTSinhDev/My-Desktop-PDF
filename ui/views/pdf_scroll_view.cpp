#include <mdpdf/ui/views/pdf_scroll_view.hpp>

#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

namespace mdpdf::ui {

PdfScrollView::PdfScrollView(QWidget* parent) : QAbstractScrollArea(parent) {
  setWidget(new QWidget);
  widget()->setAutoFillBackground(false);
  viewport()->setMouseTracking(true);
}

void PdfScrollView::set_document(core::DocumentHandle handle, int page_count) {
  document_ = handle;
  page_count_ = page_count;
  tiles_.assign(static_cast<size_t>(page_count), PageTile{});
  for (int i = 0; i < page_count; ++i) {
    tiles_[static_cast<size_t>(i)].page.value = i;
    tiles_[static_cast<size_t>(i)].layout_width = 595.0;
    tiles_[static_cast<size_t>(i)].layout_height = 842.0;
  }
  verticalScrollBar()->setValue(0);
  update_scrollbars();
  emit_visible_range();
  update();
}

void PdfScrollView::clear_document() {
  document_ = core::kInvalidDocumentHandle;
  page_count_ = 0;
  tiles_.clear();
  update();
}

void PdfScrollView::set_page_layout(core::PageIndex page,
                                  const core::PageLayout& layout) {
  if (page.value < 0 || page.value >= page_count_) {
    return;
  }
  auto& tile = tiles_[static_cast<size_t>(page.value)];
  tile.layout_width = layout.width_points;
  tile.layout_height = layout.height_points;
  update_scrollbars();
}

void PdfScrollView::set_page_image(core::PageIndex page, const QImage& image) {
  if (page.value < 0 || page.value >= page_count_) {
    return;
  }
  auto& tile = tiles_[static_cast<size_t>(page.value)];
  tile.image = image;
  tile.loading = false;
  update();
}

void PdfScrollView::set_scale(double scale) {
  scale_ = std::clamp(scale, 0.25, 4.0);
  update_scrollbars();
  emit zoom_changed(scale_);
  emit_visible_range();
  update();
}

void PdfScrollView::set_search_highlights(const std::vector<core::SearchHit>& hits) {
  for (auto& tile : tiles_) {
    tile.highlights.clear();
  }
  for (const auto& hit : hits) {
    if (hit.page.value >= 0 && hit.page.value < page_count_) {
      tiles_[static_cast<size_t>(hit.page.value)].highlights.insert(
          tiles_[static_cast<size_t>(hit.page.value)].highlights.end(),
          hit.rects.begin(), hit.rects.end());
    }
  }
  update();
}

void PdfScrollView::set_active_hit(int index) { active_hit_index_ = index; }

int PdfScrollView::visible_page_count() const { return page_count_; }

std::pair<int, int> PdfScrollView::visible_page_range() const {
  const int first = page_at_y(verticalScrollBar()->value());
  const int last = page_at_y(verticalScrollBar()->value() + viewport()->height());
  return {std::max(0, first - 2), std::min(page_count_ - 1, last + 2)};
}

void PdfScrollView::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(viewport());
  painter.fillRect(viewport()->rect(), QColor(60, 60, 60));

  const int x_offset = std::max(0, (viewport()->width() - static_cast<int>(595 * scale_)) / 2);
  const int scroll_y = verticalScrollBar()->value();

  for (int i = 0; i < page_count_; ++i) {
    const auto& tile = tiles_[static_cast<size_t>(i)];
    const int y = static_cast<int>(page_y_offset(i)) - scroll_y;
    const int w = static_cast<int>(tile.layout_width * scale_);
    const int h = static_cast<int>(tile.layout_height * scale_);
    if (y + h < 0 || y > viewport()->height()) {
      continue;
    }

    QRect target(x_offset, y, w, h);
    if (!tile.image.isNull()) {
      painter.drawImage(target, tile.image);
    } else {
      painter.fillRect(target, QColor(90, 90, 90));
      painter.setPen(Qt::white);
      painter.drawText(target, Qt::AlignCenter,
                       QString("Page %1").arg(i + 1));
    }

    for (const auto& rect : tile.highlights) {
      QRectF hr(x_offset + rect.left * scale_, y + rect.top * scale_,
                (rect.right - rect.left) * scale_,
                (rect.bottom - rect.top) * scale_);
      painter.fillRect(hr, QColor(255, 255, 0, 80));
    }

    if (selecting_ && selection_page_ == i && selection_anchor_ >= 0 &&
        selection_end_ >= 0) {
      painter.setPen(QPen(QColor(0, 120, 215), 1));
      painter.setBrush(QColor(0, 120, 215, 40));
      // Selection rects drawn by parent via selection service callback
    }
  }
}

void PdfScrollView::resizeEvent(QResizeEvent* event) {
  QAbstractScrollArea::resizeEvent(event);
  update_scrollbars();
  emit_visible_range();
}

void PdfScrollView::wheelEvent(QWheelEvent* event) {
  if (event->modifiers() & Qt::ControlModifier) {
    const auto delta = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    set_scale(scale_ * delta);
    event->accept();
    return;
  }
  QAbstractScrollArea::wheelEvent(event);
}

void PdfScrollView::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    const int page = page_at_y(verticalScrollBar()->value() + event->pos().y());
    selection_page_ = page;
    selecting_ = true;
    selection_anchor_ = 0;
    selection_end_ = 0;
    emit page_clicked(page);
  }
  QAbstractScrollArea::mousePressEvent(event);
}

void PdfScrollView::mouseMoveEvent(QMouseEvent* event) {
  if (selecting_) {
    selection_end_ = 1;
  }
  QAbstractScrollArea::mouseMoveEvent(event);
}

void PdfScrollView::mouseReleaseEvent(QMouseEvent* event) {
  if (selecting_ && selection_page_ >= 0) {
    const QPointF page_pos = map_to_page_coords(selection_page_, event->pos());
    emit selection_requested(selection_page_, page_pos.x(), page_pos.y());
  }
  selecting_ = false;
  QAbstractScrollArea::mouseReleaseEvent(event);
}

void PdfScrollView::update_scrollbars() {
  int total_height = 0;
  int max_width = 0;
  for (const auto& tile : tiles_) {
    total_height += static_cast<int>(tile.layout_height * scale_) +
                    static_cast<int>(spacing_);
    max_width = std::max(max_width, static_cast<int>(tile.layout_width * scale_));
  }
  widget()->setMinimumSize(max_width, total_height);
  verticalScrollBar()->setPageStep(viewport()->height());
  verticalScrollBar()->setSingleStep(40);
}

double PdfScrollView::page_y_offset(int page) const {
  double y = spacing_;
  for (int i = 0; i < page; ++i) {
    y += tiles_[static_cast<size_t>(i)].layout_height * scale_ + spacing_;
  }
  return y;
}

int PdfScrollView::page_at_y(double y) const {
  for (int i = 0; i < page_count_; ++i) {
    const double top = page_y_offset(i);
    const double bottom =
        top + tiles_[static_cast<size_t>(i)].layout_height * scale_;
    if (y >= top && y < bottom) {
      return i;
    }
  }
  return std::max(0, page_count_ - 1);
}

QPointF PdfScrollView::map_to_page_coords(int page, const QPoint& pos) const {
  const int scroll_y = verticalScrollBar()->value();
  const int x_offset =
      std::max(0, (viewport()->width() - static_cast<int>(595 * scale_)) / 2);
  const double x = (pos.x() - x_offset) / scale_;
  const double y = (pos.y() + scroll_y - page_y_offset(page)) / scale_;
  return {x, y};
}

void PdfScrollView::emit_visible_range() {
  const auto [first, last] = visible_page_range();
  emit visible_range_changed(first, last);
}

}  // namespace mdpdf::ui
