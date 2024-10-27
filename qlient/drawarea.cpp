#include "drawarea.h"
#include <fmt/core.h>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPath>
#include "game.h"
#include "table.h"

DrawArea::DrawArea(Table *table) :
  QWidget(table),
  table_{*table} {
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
  qDebug() << fmt::format("DrawArea::paintEvent w={}, h={}", w, h);
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
      DrawCard(game->cards_active_[ai], card_rect);
    }
  }
}

void DrawArea::DrawCard(
    unsigned card,
    const QRect &card_rect) {
  static const QColor white(0xff, 0xff, 0xff);
  QPainter painter(this);
  painter.fillRect(card_rect, white);
  auto qr = std::div(card, 3);
  unsigned shading = qr.rem;
  qr = std::div(qr.quot, 3);
  unsigned color = qr.rem;
  qr = std::div(qr.quot, 3);
  unsigned symbol = qr.rem;
  unsigned num_symbols = qr.quot + 1;
  qDebug() << fmt::format(
    "card={}, shading={} color={} symbol={}, num_symbols={}\n",
    card, shading, color, symbol, num_symbols);
  static std::array<void (DrawArea::*)(
      const QRect&, unsigned, unsigned, bool), 3> 
    draw_sym_funcs =
      {&DrawArea::DrawDiamond, &DrawArea::DrawSquiggle, &DrawArea::DrawOval};
  auto draw_symbol = draw_sym_funcs[symbol];

  unsigned sym_w = (3*card_rect.width())/4;
  unsigned sym_h = (3*card_rect.height())/4;
  unsigned x_gap = (card_rect.width() - num_symbols*sym_w) / (num_symbols + 1);
  unsigned y_gap = (card_rect.height() - sym_h)/2;
  QRect symbol_rect(card_rect.x() + x_gap, card_rect.y() + y_gap, sym_w, sym_h);
  for (unsigned r = 0; r < num_symbols; ++r) {
    for (bool fill_pass: shading_fill_passes_[shading]) {
      ((*this).*draw_symbol)(symbol_rect, shading, color, fill_pass);
    }
    symbol_rect.setX(symbol_rect.x() + sym_w + x_gap);
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
  color_brushes_[ci] = QBrush(8, pattern);
}

void DrawArea::DrawDiamond(
    const QRect& rect,
    unsigned shading,
    unsigned color,
    bool fill_pass) {
  QPainter painter(this);
  QPainterPath diamond;
  diamond.moveTo(rect.x() + rect.width()/2, rect.y());
  diamond.lineTo(rect.x() + rect.width(), rect.y() + rect.height()/2);
  diamond.lineTo(rect.x() + rect.width()/2, rect.y() + rect.height());
  diamond.lineTo(rect.x(), rect.y() + rect.height()/2);
  diamond.closeSubpath();
  if (fill_pass) {
    painter.fillPath(diamond, QBrush(card_colors_[color]));
  } else {
    painter.setPen(card_colors_[color]);
    painter.drawPath(diamond);
  }
}

void DrawArea::DrawSquiggle(
    const QRect& symbol_rect,
    unsigned shading,
    unsigned color,
    bool fill_pass) {
}

void DrawArea::DrawOval(
    const QRect& symbol_rect,
    unsigned shading,
    unsigned color,
    bool fill_pass) {
}
