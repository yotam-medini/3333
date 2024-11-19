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
#include "utils.h"

Table::Table(QWidget *parent) :
    QWidget(parent) {
  setStyleSheet("background-color:#392;");
  QWidget *wrap;
  QVBoxLayout *vlayout = new QVBoxLayout();
  QHBoxLayout *hlayout_top = new QHBoxLayout();
  QHBoxLayout *hlayout_stats = new QHBoxLayout();
  name_at_name_ = new QLabel("<b>?@?</b>", this);
  hlayout_top->addWidget(name_at_name_);
  butt_new_game_ = new QPushButton("New Game", this);
  connect(butt_new_game_, &QPushButton::clicked, [this]() {
    this->new_game_func_(); });
  butt_new_game_->setEnabled(false);
  hlayout_top->addWidget(butt_new_game_);
  butt_add3_nomore_ = new QPushButton("Add 3", this);
  hlayout_top->addWidget(butt_add3_nomore_);
  connect(butt_add3_nomore_, &QPushButton::clicked, [this]() {
    this->add3_nomore_func_(); });
  butt_add3_nomore_->setEnabled(false);
  wrap = new QWidget(this);
  wrap->setLayout(hlayout_top);
  vlayout->addWidget(wrap);

  picked_ = new QLabel("0 picked", this);
  hlayout_stats->addWidget(picked_);
  status_summary_ = new QLabel("# players, # found, # @deck ", this);
  status_summary_->setToolTip("Status Summary");
  hlayout_stats->addWidget(status_summary_);

  setStyleSheet("QToolTip {color: #000; background-color: #fff;}");
  const QString style_good{"QLabel {background-color: white; color: #0a0; }"};
  const QString style_bad{"QLabel {background-color: white; color: #a00; }"};

  set_calls_good_ = new QLabel("0", this);
  set_calls_good_->setStyleSheet(style_good);
  set_calls_good_->setToolTip("Good Set calls");
  hlayout_stats->addWidget(set_calls_good_);
  set_calls_bad_ = new QLabel("0", this);
  set_calls_bad_->setStyleSheet(style_bad);
  set_calls_bad_->setToolTip("Bad Set calls");
  hlayout_stats->addWidget(set_calls_bad_);

  add3_good_ = new QLabel("0", this);
  add3_good_->setStyleSheet(style_good);
  add3_good_->setToolTip("Good Add3 calls");
  hlayout_stats->addWidget(add3_good_);
  add3_bad_ = new QLabel("0", this);
  add3_bad_->setStyleSheet(style_bad);
  add3_bad_->setToolTip("Bad Add3 calls");
  hlayout_stats->addWidget(add3_bad_);

  no_more_good_ = new QLabel("0", this);
  no_more_good_->setStyleSheet(style_good);
  no_more_good_->setToolTip("Good No more calls");
  hlayout_stats->addWidget(no_more_good_);
  no_more_bad_ = new QLabel("0", this);
  no_more_bad_->setStyleSheet(style_bad);
  no_more_bad_->setToolTip("Bad No more calls");
  hlayout_stats->addWidget(no_more_bad_);

  score_ = new QLabel("Score: <b>0</b>", this);
  score_->setToolTip("Total: good-bad");
  hlayout_stats->addWidget(score_);

  wrap = new QWidget(this);
  wrap->setLayout(hlayout_stats);
  vlayout->addWidget(wrap);

  draw_area_ = new DrawArea(
    this, 
    [this]() { this->UpdateSelected(); },
    selected_);
  draw_area_->setStyleSheet("background-color:#9ab;");
  vlayout->addWidget(draw_area_, 2);
  setLayout(vlayout);
}

Table::~Table() {
}

void Table::NewTable(const QVariantMap &result_map) {
  qDebug() << "Table::NewTable";
  std::string name = result_map["table_name"].toString().toStdString();
  player_name_ = name;
  qDebug() << fmt::format("name={}", name);
  std::string t = fmt::format("<b>{}@{}</b>", player_name_, name);
  name_at_name_->setText(QString::fromStdString(t));
  butt_new_game_->setEnabled(true);
}

void Table::JoinTable(const QVariantMap &result_map) {
  qDebug() << "Table::NewTable";
  player_name_ = result_map["player_name"].toString().toStdString();
  std::string table_name = result_map["table_name"].toString().toStdString();
  qDebug() << fmt::format("player/table {}/{}", player_name_, table_name);
  std::string t = fmt::format("<b>{}@{}</b>", player_name_, table_name);
  qDebug() << t;
  name_at_name_->setText(QString::fromStdString(t));
  butt_new_game_->setEnabled(false);
}

void Table::SetGame(const Game& game) {
  game_ = &game;
  butt_add3_nomore_->setEnabled(game.active_);
  status_summary_->setText(QString::fromStdString(fmt::format(
    "{} players, {} found, {} @deck",
    game.players_.size(), game.sets_found_, game.deck_)));
  butt_add3_nomore_->setText(game.deck_ != 0 ? "Add 3" : "No More");
  auto iter = std::find_if(game.players_.begin(), game.players_.end(),
    [this](const Player& p) -> bool {
      return p.name_ == this->player_name_;
    });
  if (iter != game.players_.end()) {
    const Player &player = *iter;
    auto label_set = [](QLabel *label, int n) -> void {
      const std::string s = fmt::format("<b>{}</b>", n);
      label->setText(QString::fromStdString(s));
    };
    label_set(set_calls_good_, player.set_calls_good_);
    label_set(set_calls_bad_, player.set_calls_bad_);
    label_set(add3_good_, player.add3_good_);
    label_set(add3_bad_, player.add3_bad_);
    label_set(no_more_good_, player.no_more_good_);
    label_set(no_more_bad_, player.no_more_bad_);
    label_set(score_, player.score());
  }
  selected_.clear();
  UpdateSelected();
  draw_area_->update();
}


void Table::UpdateSelected() {
  qDebug() << "Table::UpdateSelected";
  std::string text = fmt::format("{} picked", selected_.size());
  picked_->setText(QString::fromStdString(text));
  if (selected_.size() == 3) {
    try3_func_(selected_);
  }
}
