#include "table.h"
#include <cstdlib>
#include <fmt/core.h>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include "game.h"
#include "drawarea.h"

Table::Table(QWidget *parent) :
    QWidget(parent) {
  setStyleSheet("background-color:#392;");
  QVBoxLayout *vlayout = new QVBoxLayout(parent);
  QHBoxLayout *hlayout = new QHBoxLayout(parent);
  name_at_name_ = new QLabel("<b>?@?</b>", this);
  hlayout->addWidget(name_at_name_);
  butt_new_game_ = new QPushButton("New Game", this);
  connect(butt_new_game_, &QPushButton::clicked, [this]() {
    this->new_game_func_(); });
  ///butt_add3_nomore_ = std::make_unique<QPushButton>("Add 3", this);
  hlayout->addWidget(butt_new_game_);
  vlayout->addLayout(hlayout);
  draw_area_ = new DrawArea(this);
  draw_area_->setStyleSheet("background-color:#9ab;");
  vlayout->addWidget(draw_area_);
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
