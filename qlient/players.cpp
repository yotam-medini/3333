#include "players.h"
#include <string>
#include <fmt/core.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include "game.h"
#include "icondata.h"
#include "player.h"
#include "svglabel.h"
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
  const std::vector<Player> &players = game.players_;
  bool same_create_ymd = true;
  bool same_action_ymd = true;
  if (!players.empty()) {
    const std::string ymd_created{ymd(players[1].tcreated_)};
    const std::string ymd_action{ymd(players[1].taction_)};
    for (size_t i = 1;
        (i < players.size()) && (same_create_ymd || same_action_ymd); ++i) {
      same_create_ymd =
        same_create_ymd && (ymd_created == ymd(players[i].tcreated_));
      same_action_ymd =
        same_action_ymd && (ymd_action == ymd(players[i].taction_));
    }
  }
  using time_func_t = std::string(*)(unsigned);
  time_func_t created_tfunc = (same_create_ymd ? &hms : &ymdhms);
  time_func_t action_tfunc = (same_action_ymd ? &hms : &ymdhms);
  for (const Player &player: game.players_) {
    std::string name = player.name_;
    if (name == table_name_) {
      name = fmt::format("<b>{}</b>", name);
    }
    grid_->addWidget(new QLabel(QString::fromStdString(name), this),
                     row, 0);
    grid_->addWidget(
      new QLabel(QString::fromStdString(
        (*created_tfunc)(player.tcreated_)), this), row, 1);
    grid_->addWidget(
      new QLabel(QString::fromStdString(
        (*action_tfunc)(player.taction_)), this), row, 2);
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
      row, 8, Qt::AlignRight);
    grid_->addWidget(new QLabel(QString::number(player.no_more_bad_), this),
      row, 9, Qt::AlignRight);
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
  // grid_->addWidget(new_bold_label("Good"),    1, 4, 1, 1);
  grid_->addWidget(new SvgLabel(smile_data, this), 1, 4, // 1, 1,
    Qt::AlignCenter);
  // grid_->setRowStretch(1, 1);
  // grid_->setColumnStretch(4, 1);
  // grid_->addWidget(new_bold_label("Wrong"),   1, 5, 1, 1);
  grid_->addWidget(new SvgLabel(frown_data, this), 1, 5, Qt::AlignCenter);

  grid_->addWidget(new_bold_label("Add3"),    0, 6, 1, 2);
  grid_->addWidget(new SvgLabel(smile_data, this), 1, 6, Qt::AlignCenter);
  grid_->addWidget(new SvgLabel(frown_data, this), 1, 7, Qt::AlignCenter);

  grid_->addWidget(new_bold_label("No More"), 0, 8, 1, 2);
  grid_->addWidget(new SvgLabel(smile_data, this), 1, 8, Qt::AlignCenter);
  grid_->addWidget(new SvgLabel(frown_data, this), 1, 9, Qt::AlignCenter);
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
