#include "table.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

Table::Table(QWidget *parent) :
    QWidget(parent) {
  setStyleSheet("background-color:#392;");
  QVBoxLayout *vlayout = new QVBoxLayout(parent);
  QHBoxLayout *hlayout = new QHBoxLayout(parent);
  name_at_name_ = std::make_unique<QLabel>("<b>?@?</b>", this);
  hlayout->addWidget(name_at_name_.get());
  butt_new_game = std::make_unique<QPushButton>("New Game", this);
  hlayout->addWidget(butt_new_game.get());
  vlayout->addLayout(hlayout);
  draw_area_ = std::make_unique<QWidget>(this);
  draw_area_->setStyleSheet("background-color:#222;");
  vlayout->addWidget(draw_area_.get());
  setLayout(vlayout);
}

Table::~Table() {
}
