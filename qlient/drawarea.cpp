#include "drawarea.h"
#include <fmt/core.h>
#include <QPainter>
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
      DrawCard(painter, game->cards_active_[ai], card_rect);
    }
  }
}

void DrawArea::DrawCard(
    QPainter &painter,
    unsigned card,
    const QRect &card_rect) {
  static const QColor white(0xff, 0xff, 0xff);
  painter.fillRect(card_rect, white);
  auto qr = std::div(card, 3);
  unsigned shading = qr.rem;
  qr = std::div(qr.quot, 3);
  unsigned color = qr.rem;
  qr = std::div(qr.quot, 3);
  unsigned symbol = qr.rem;
  unsigned n_symbols = qr.quot + 1;
  qDebug() << fmt::format(
    "card={}, shading={} color={} symbol={}, n_symbols={}\n",
    card, shading, color, symbol, n_symbols);
  static std::array<void (DrawArea::*)(const QRect&, unsigned, unsigned), 3> 
    draw_sym_funcs =
      {&DrawArea::DrawDiamond, &DrawArea::DrawSquiggle, &DrawArea::DrawOval};
  auto draw_symbol = draw_sym_funcs[symbol];
  ((*this).*draw_symbol)(QRect(), shading, color);
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

