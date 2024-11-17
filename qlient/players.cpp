#include "players.h"
#include <string>
#include <fmt/core.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include "game.h"
#include "player.h"
#include "utils.h"

Players::Players(QWidget *parent) : QWidget(parent) {
  qDebug("Club::Club");
  QVBoxLayout *vlayout = new QVBoxLayout();
  QWidget *bar = new QWidget(this);
  QHBoxLayout *hlayout = new QHBoxLayout();
  hlayout->addWidget(new QWidget(this), 1); // left space
  title_ = new QLabel("<b>Table Players</b>", this);
  hlayout->addWidget(title_, 0);
  hlayout->addWidget(new QWidget(this), 1); // right space
  bar->setLayout(hlayout);
  vlayout->addWidget(bar);

  grid_ = new QGridLayout(this);
  grid_->setSpacing(6);
  SetGridTitleRow();
  QScrollArea *scrollArea = new QScrollArea;
  QWidget *grid_wrap = new QWidget(this);
  grid_wrap->setStyleSheet("background-color: pink;");
  grid_wrap->setLayout(grid_);
  scrollArea->setWidget(grid_wrap);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setWidgetResizable(true);
  vlayout->addWidget(scrollArea, 1);

  setLayout(vlayout);  
}

Players::~Players() {
  qDebug("Club::~Club");
}

void Players::NewTable(const QVariantMap &result_map) {
  table_name_ = result_map["table_name"].toString().toStdString();
  auto text = fmt::format("<b>Players of {}", table_name_);
  title_->setText(QString::fromStdString(text));
}

void Players::UpdatePlayers(const Game& game) {
  ClearGridData();
  int row = 2;
  for (const Player &player: game.players_) {
    std::string name = player.name_;
    if (name == table_name_) {
      name = fmt::format("<b>{}</b>", name);
    }
    grid_->addWidget(new QLabel(QString::fromStdString(name), this),
                     row, 0);
    grid_->addWidget(
      new QLabel(QString::fromStdString(ymdhms(player.tcreated_)), this),
      row, 1);
    grid_->addWidget(
      new QLabel(QString::fromStdString(ymdhms(player.taction_)), this),
      row, 2);
    grid_->addWidget(new QLabel(QString::number(player.score()), this),
      row, 3, Qt::AlignRight);
    grid_->addWidget(new QLabel(QString::number(player.set_calls_good_), this),
      row, 4, Qt::AlignRight);
    grid_->addWidget(new QLabel(QString::number(player.set_calls_bad_), this),
      row, 5, Qt::AlignRight);
    grid_->addWidget(new QLabel(QString::number(player.add3_good_), this),
      row, 6, Qt::AlignRight);
    grid_->addWidget(new QLabel(QString::number(player.add3_bad_), this),
      row, 7, Qt::AlignRight);
    grid_->addWidget(new QLabel(QString::number(player.no_more_good_), this),
      row, 6, Qt::AlignRight);
    grid_->addWidget(new QLabel(QString::number(player.no_more_bad_), this),
      row, 7, Qt::AlignRight);
    ++row;
  }
}

void Players::SetGridTitleRow() {
  auto new_bold_label = [this](const std::string &text) -> QLabel* {
    const std::string btextb = fmt::format("<b>{}</b>", text);
    QLabel *label = new QLabel(QString::fromStdString(btextb), this);
    label->setStyleSheet("background-color: white;");
    return label;
  };
  grid_->addWidget(new_bold_label("Name"),    0, 0, 2, 1);
  grid_->addWidget(new_bold_label("Created"), 0, 1, 2, 1);
  grid_->addWidget(new_bold_label("Action"),  0, 2, 2, 1);
  grid_->addWidget(new_bold_label("Score"),   0, 3, 2, 1);

  grid_->addWidget(new_bold_label("Sets"),    0, 4, 1, 2);
  grid_->addWidget(new_bold_label("Good"),    1, 4, 1, 1);
  grid_->addWidget(new_bold_label("Wrong"),   1, 5, 1, 1);

  grid_->addWidget(new_bold_label("Add3"),    0, 6, 1, 2);
  grid_->addWidget(new_bold_label("Good"),    1, 6, 1, 1);
  grid_->addWidget(new_bold_label("Wrong"),   1, 7, 1, 1);

  grid_->addWidget(new_bold_label("No More"), 0, 8, 1, 2);
  grid_->addWidget(new_bold_label("Good"),    1, 8, 1, 1);
  grid_->addWidget(new_bold_label("Wrong"),   1, 9, 1, 1);
}

void Players::ClearGridData() {
  const int rows_count = grid_->rowCount();
  const int cols_count = grid_->columnCount();
  // Leave the title rows
  for (int row = rows_count - 1; row >= 2; --row) {
    for (int col = cols_count - 1; col >= 0; --col) {
      QLayoutItem* item = grid_->itemAtPosition(row, col);
      if (item) {
        QWidget *widget = item->widget();
        grid_->removeItem(item);
        delete item;
        if (widget) {
           widget->deleteLater();  // Schedule widget for deletion
        }
      }
    }
  }
}
