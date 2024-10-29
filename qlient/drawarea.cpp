#include "drawarea.h"
#include <algorithm>
#include <fmt/core.h>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPath>
#include "game.h"
#include "table.h"

DrawArea::DrawArea(Table *table, std::vector<unsigned> &selected) :
  QWidget(table),
  table_{*table},
  selected_{selected} {
}

const std::vector<bool>  DrawArea::shading_fill_passes_[3] = {
  std::vector<bool>{true},        // filled
  std::vector<bool>{true, false}, // stripped
  std::vector<bool>{false}        // open
};

const QColor DrawArea::card_colors_[3] = {
  QColor{"#e21"}, // ~red
  QColor{"#382"}, // ~green
  QColor{"#a3f"} // ~purple
};

void DrawArea::paintEvent(QPaintEvent *event) {
  using au2_t = Golden::au2_t;
  int w = width();
  int h = height();
  // qDebug() << fmt::format("DrawArea::paintEvent w={}, h={}", w, h);
  QPainter painter(this);
  painter.setPen(QColor(0x22, 0xaa, 0x33));
  painter.drawRect(0, 0, w, h);
  const Game *game = table_.GetGame();
  if (game) {
    auto const &cards = game->cards_active_;
    golden_.Set(w, h, cards.size());
    const au2_t card_size = golden_.GetCardSize();
    SetBrushes(card_size[1]);
    QColor white(0xff, 0xff, 0xff);
    for (size_t ai = 0; ai < cards.size(); ++ai) {
      const au2_t pos = golden_.GetCardPosition(ai);
      QRect card_rect(pos[0], pos[1], card_size[0], card_size[1]);
      DrawCard(game->cards_active_[ai], card_rect,
        std::find(selected_.begin(), selected_.end(), ai) != selected_.end());
    }
  }
}

void DrawArea::DrawCard(
    unsigned card,
    const QRect &card_rect,
    bool picked) {
  static const QColor white(0xff, 0xff, 0xff);
  static const QColor gold(0xff, 0xd7, 0x00);
  {
    QPainter painter(this);
    painter.fillRect(card_rect, white);
  }
  if (picked) {
    QPainter painter(this);
    QPen pen;
    pen.setColor(gold);
    pen.setWidth(std::max<int>(4, card_rect.width()/24));
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRect(card_rect);
  }
  auto qr = std::div(card, 3);
  unsigned shading = qr.rem;
  qr = std::div(qr.quot, 3);
  unsigned color = qr.rem;
  qr = std::div(qr.quot, 3);
  unsigned symbol = qr.rem;
  unsigned num_symbols = qr.quot + 1;
  // qDebug() << fmt::format(
  //   "card={}, shading={} color={} symbol={}, num_symbols={}\n",
  //  card, shading, color, symbol, num_symbols);
  static std::array<QPainterPath (DrawArea::*)(const QRect&) const, 3> 
    get_sym_path_funcs = {
      &DrawArea::GetDiamondPath, 
      &DrawArea::GetSquigglePath,
      &DrawArea::GetOvalPath};
  auto get_symbol_path = get_sym_path_funcs[symbol];

  int sym_w = card_rect.width()/4;
  int sym_h = (3*card_rect.height())/4;
  const QSize sym_size{sym_w, sym_h};
  int x_gap = (card_rect.width() - num_symbols*sym_w) / (num_symbols + 1);
  int y_gap = (card_rect.height() - sym_h)/2;
  QPoint left_top{card_rect.x() + x_gap, card_rect.y() + y_gap};
  // QRect symbol_rect(card_rect.x() + x_gap, card_rect.y() + y_gap, sym_w, sym_h);
  for (unsigned r = 0; r < num_symbols; ++r) {
    QRect symbol_rect;
    symbol_rect.setTopLeft(left_top);
    symbol_rect.setSize(sym_size);
    QPainterPath path = ((*this).*get_symbol_path)(symbol_rect);
    for (bool fill_pass: shading_fill_passes_[shading]) {
      QPainter painter(this);
      painter.setRenderHint(QPainter::Antialiasing);
      if (fill_pass) {
        painter.fillPath(path, shading == 1
          ? color_stripes_brushes_[color] : QBrush(card_colors_[color]));
      } else {
        QPen pen;
        pen.setColor(card_colors_[color]);
        pen.setWidth(std::max<int>(4, sym_size.width()/12));
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setPen(pen);
        painter.drawPath(path);
      }
    }
    symbol_rect.setX(symbol_rect.x() + sym_w + x_gap);
    left_top.setX(left_top.x() + sym_w + x_gap);
  }
}

void DrawArea::mouseReleaseEvent(QMouseEvent *event) {
  auto position = event->position();
  // qDebug() << fmt::format("DrawArea::mouseReleaseEvent: {},{}",
  //  position.x(), position.y());
  const Game *game = table_.GetGame();
  if (game && !game->cards_active_.empty()) {
    unsigned who = golden_.Pick(position.x(), position.y());
    qDebug() << fmt::format("DrawArea:: who={}", who);
    if (who != unsigned(-1)) {
      auto iter = std::find(selected_.begin(), selected_.end(), who);
      if (iter == selected_.end()) {
        if (selected_.size() < 3) {
          selected_.push_back(who);
        }
      } else { // unselect
        *iter = selected_.back();
        selected_.pop_back();
      }
      update();
    }
  }
}

void DrawArea::SetBrushes(unsigned card_height) {
  if (stripe_height_ != card_height/12) {
    stripe_height_ = card_height/12;
    for (unsigned ci: {0, 1, 2}) {
      SetBrush(ci);
    }
  }
}

void DrawArea::SetBrush(unsigned ci) {
  QPixmap pattern(8, 2*stripe_height_);
  pattern.fill(QColor(0xff, 0xff, 0xff));
  QPainter painter(&pattern);
  painter.fillRect(0, 0, 8, stripe_height_, card_colors_[ci]);
  color_stripes_brushes_[ci] = QBrush(8, pattern);
}

QPainterPath DrawArea::GetDiamondPath(const QRect& rect) const {
  QPainterPath diamond;
  diamond.moveTo(rect.x() + rect.width()/2, rect.y());
  diamond.lineTo(rect.x() + rect.width(), rect.y() + rect.height()/2);
  diamond.lineTo(rect.x() + rect.width()/2, rect.y() + rect.height());
  diamond.lineTo(rect.x(), rect.y() + rect.height()/2);
  diamond.closeSubpath();
  return diamond;
}

QPainterPath DrawArea::GetSquigglePath(const QRect& rect) const {
  const int w = rect.width();
  const int h = rect.height();
  int rel_curves_left[4][3][2] = {
    {{w/8, 0},      {0,   h/16},   {0,   h/14}},
    {{w/3, h/8},    {w/4, h/4},    {w/5, 3*h/8}},
    {{w/5, h/2},    {0,   5*h/8},  {0,   3*h/4}},
    {{0,   7*h/8},  {w/4, h},      {w/2, h}}
  };
  int rel_curves_right[4][3][2];
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      // center symmetry
      rel_curves_right[i][j][0] = w - rel_curves_left[i][j][0];
      rel_curves_right[i][j][1] = h - rel_curves_left[i][j][1];
    }
  }
  const int x = rect.x();
  const int y = rect.y();
  int curves[8][3][2];
  int ci = 0;
  for (const auto &prel: {rel_curves_left, rel_curves_right}) {
    for (size_t i = 0; i < 4; ++i, ++ci) {
      for (size_t j = 0; j < 3; ++j) {
        curves[ci][j][0] = x + prel[i][j][0];
        curves[ci][j][1] = y + prel[i][j][1];
      }
    }
  }
  QPainterPath path;
  // The last point is also the first!
  path.moveTo(curves[7][2][0], curves[7][2][1]);
  for (size_t i = 0; i < 8; ++i) {
    auto const &p = curves[i];
    path.cubicTo(
      p[0][0], p[0][1], 
      p[1][0], p[1][1], 
      p[2][0], p[2][1]);
  }
  return path;
}

QPainterPath DrawArea::GetOvalPath(const QRect& rect) const {
  QPainterPath path;
  path.addEllipse(rect);
  return path;
}
