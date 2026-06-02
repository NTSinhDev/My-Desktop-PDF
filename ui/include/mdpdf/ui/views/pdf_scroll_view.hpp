#pragma once

#include <mdpdf/core/domain/document_models.hpp>

#include <QAbstractScrollArea>
#include <QImage>
#include <QPoint>
#include <QRectF>
#include <unordered_map>
#include <vector>

namespace mdpdf::ui {

struct PageTile {
  core::PageIndex page;
  double layout_height = 0.0;
  double layout_width = 0.0;
  QImage image;
  bool loading = false;
  std::vector<core::Rect> highlights;
};

class PdfScrollView : public QAbstractScrollArea {
  Q_OBJECT

public:
  explicit PdfScrollView(QWidget* parent = nullptr);

  void set_document(core::DocumentHandle handle, int page_count);
  void clear_document();
  void set_page_layout(core::PageIndex page, const core::PageLayout& layout);
  void set_page_image(core::PageIndex page, const QImage& image);
  void set_scale(double scale);
  double scale() const { return scale_; }

  void set_search_highlights(const std::vector<core::SearchHit>& hits);
  void set_active_hit(int index);

  int visible_page_count() const;
  std::pair<int, int> visible_page_range() const;

signals:
  void visible_range_changed(int first_page, int last_page);
  void page_clicked(int page);
  void selection_changed(const core::TextSelection& selection);
  void selection_requested(int page, double x, double y);
  void zoom_changed(double scale);

protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  void update_scrollbars();
  double page_y_offset(int page) const;
  int page_at_y(double y) const;
  QPointF map_to_page_coords(int page, const QPoint& pos) const;
  void emit_visible_range();

  core::DocumentHandle document_ = core::kInvalidDocumentHandle;
  int page_count_ = 0;
  double scale_ = 1.0;
  double spacing_ = 8.0;
  std::vector<PageTile> tiles_;
  int active_hit_index_ = -1;

  bool selecting_ = false;
  int selection_page_ = -1;
  int selection_anchor_ = -1;
  int selection_end_ = -1;
};

}  // namespace mdpdf::ui
