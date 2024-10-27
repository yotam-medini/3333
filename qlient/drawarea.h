#pragma once

#include <vector>
#include <QWidget>
#include "golden.h"

class QPaintEvent;
class QPainter;
class QRect;
class Table;

class DrawArea : public QWidget {
 public:
  DrawArea(Table *table);
 protected:
  void paintEvent(QPaintEvent *event) override;
 private:
  void DrawCard(QPainter &painter, unsigned card, const QRect &card_rect);
  void DrawDiamond(const QRect& symbol_rect, unsigned shading, unsigned color);
  void DrawSquiggle(const QRect& symbol_rect, unsigned shading, unsigned color);
  void DrawOval(const QRect& symbol_rect, unsigned shading, unsigned color);
  void SetBrushes(unsigned card_height);
  void SetBrush(unsigned ci);
  static const std::vector<bool> shading_fill_passes_[3];
  static const QColor card_colors_[3]; // red, green, purple
  QBrush color_brushes_[3];
  unsigned stripe_height_{0};
  const Table &table_;
  Golden golden_;
};
