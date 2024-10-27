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
  QPainter ell(this);
  ell.setPen(QColor(0x22, 0xaa, 0x33));
  ell.drawRect(0, 0, w, h);
  const Game *game = table_.GetGame();
  if (game) {
    auto const &cards = game->cards_active_;
    golden_.Set(w, h, cards.size());
    const au2_t card_size = golden_.GetCardSize();
    SetBrushes(card_size[1]);
    QColor white(0xff, 0xff, 0xff);
    for (size_t ai = 0; ai < cards.size(); ++ai) {
      const au2_t pos = golden_.GetCardPosition(ai);
      ell.fillRect(pos[0], pos[1], card_size[0], card_size[1], white);
      size_t card = cards[ai];
      auto qr = std::div(card, 3);
      unsigned shading = qr.rem;
      qr = std::div(qr.quot, 3);
      unsigned color = qr.rem;
      unsigned symbol = qr.quot;
      unsigned symbols = symbol + 1;
      qDebug() << fmt::format(
        "ai={} card={}, shading={} color={} symbols={}\n",
        ai, card, shading, color, symbols);
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
}

