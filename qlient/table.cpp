#include "table.h"
#include <fmt/core.h>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include "game.h"
#include "golden.h"

class DrawArea : public QWidget {
 public:
  DrawArea(Table *table) :
    QWidget(table),
    table_{*table} {
  }
 protected:
  void paintEvent(QPaintEvent *event) override {
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
      golden_.set(w, h, cards.size());
      const au2_t card_size = golden_.GetCardSize();
      for (size_t ai = 0; ai < cards.size(); ++ai) {
        const au2_t pos = golden_.GetCardPosition(ai);
        ell.setPen(QColor(0xff, 0xff, 0xff));
        ell.drawRect(pos[0], pos[1], card_size[0], card_size[1]);
        size_t ci = cards[ai];
      }
    }
  }
 private:
  const Table &table_;
  Golden golden_;
};

Table::Table(QWidget *parent) :
    QWidget(parent) {
  setStyleSheet("background-color:#392;");
  QVBoxLayout *vlayout = new QVBoxLayout(parent);
  QHBoxLayout *hlayout = new QHBoxLayout(parent);
  name_at_name_ = std::make_unique<QLabel>("<b>?@?</b>", this);
  hlayout->addWidget(name_at_name_.get());
  butt_new_game = std::make_unique<QPushButton>("New Game", this);
  connect(butt_new_game.get(), &QPushButton::clicked, [this]() { this->new_game_func_(); });
  hlayout->addWidget(butt_new_game.get());
  vlayout->addLayout(hlayout);
  draw_area_ = std::make_unique<DrawArea>(this);
  draw_area_->setStyleSheet("background-color:#9ab;");
  vlayout->addWidget(draw_area_.get());
  setLayout(vlayout);
}

Table::~Table() {
}

void Table::NewTable(const QVariantMap &result_map) {
  qDebug() << "Table::NewTable";
  std::string name = result_map["table_name"].toString().toStdString();
  qDebug() << fmt::format("name={}", name);
  std::string t = fmt::format("<b>{}@{}</b>", name, name);
  name_at_name_->setText(QString::fromStdString(t));
}

void Table::SetGame(const Game& game) {
  game_ = &game;
  draw_area_->update();
}
