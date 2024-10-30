#pragma once

#include <vector>
#include <functional>
#include <QWidget>
#include <QPainterPath>
#include "golden.h"

class QPaintEvent;
class QPainter;
class QRect;
class Table;

class DrawArea : public QWidget {
 public:
  DrawArea(
    Table *table,
    std::function<void(void)> update_selected_func,
    std::vector<unsigned> &selected);
 protected:
  void paintEvent(QPaintEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
 private:
  void DrawCard(unsigned card, const QRect &card_rect, bool picked);
  QPainterPath GetDiamondPath(const QRect& symbol_rect) const;
  QPainterPath GetSquigglePath(const QRect& symbol_rect) const;
  QPainterPath GetOvalPath(const QRect& symbol_rect) const;
  void SetBrushes(unsigned card_height);
  void SetBrush(unsigned ci);
  static const std::vector<bool> shading_fill_passes_[3];
  static const QColor card_colors_[3]; // red, green, purple
  QBrush color_stripes_brushes_[3];
  unsigned stripe_height_{0};
  const Table &table_;
  Golden golden_;
  std::function<void(void)> update_selected_func_;
  std::vector<unsigned> &selected_;
};
