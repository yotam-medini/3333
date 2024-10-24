#include "table.h"
#include <fmt/core.h>
#include <QDebug>
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

void Table::NewTable(const QVariantMap &result_map) {
  qDebug() << "Table::NewTable";
  std::string name = result_map["table_name"].toString().toStdString();
  qDebug() << fmt::format("name={}", name);
  std::string t = fmt::format("<b>{}@{}</b>", name, name);
  name_at_name_->setText(QString::fromStdString(t));
}
