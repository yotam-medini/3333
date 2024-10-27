#pragma once

#include <QWidget>
#include "golden.h"

class QPaintEvent;
class Table;

class DrawArea : public QWidget {
 public:
  DrawArea(Table *table);
 protected:
  void paintEvent(QPaintEvent *event) override;
 private:
  void SetBrushes(unsigned card_height);
  void SetBrush(unsigned ci);
  static const QColor card_colors_[3]; // red, green, purple
  QBrush color_brushes_[3];
  unsigned stripe_height_{0};
  const Table &table_;
  Golden golden_;
};
