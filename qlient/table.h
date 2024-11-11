#pragma once

#include <functional>
#include <vector>
#include <QWidget>
#include "common.h"

class QLabel;
class QPushButton;
class QWidget;

class Game;
class DrawArea;

class Table : public QWidget {
 public:
  Table(QWidget *parent);
  ~Table();
  void SetName(const std::string &s);
  void NewTable(const QVariantMap &result_map);
  void SetNewGameFunc(std::function<int(void)> f) { new_game_func_ = f; }
  void SetAdd3NoMoreFunc(std::function<int(void)> f) { add3_nomore_func_ = f; }
  void SetTry3Func(std::function<int(const std::vector<unsigned>&)> f) {
    try3_func_ = f;
  }
  void SetGame(const Game& game);
  const Game *GetGame() const { return game_; }
 private:
  void UpdateSelected();
  std::string player_name_;
  QLabel *name_at_name_{nullptr};
  QPushButton *butt_new_game_{nullptr};
  QPushButton *butt_add3_nomore_{nullptr};
  QLabel *picked_{nullptr};
  QLabel *status_summary_{nullptr};
  // Player scores
  QLabel *set_calls_good_{0};
  QLabel *set_calls_bad_{0};
  QLabel *add3_good_{0};
  QLabel *add3_bad_{0};
  QLabel *no_more_good_{0};
  QLabel *no_more_bad_{0};
  QLabel *score_{0};
  
  DrawArea *draw_area_{nullptr};
  std::function<int(void)> new_game_func_{nullptr};
  std::function<int(void)> add3_nomore_func_{nullptr};
  std::function<int(const std::vector<unsigned>&)> try3_func_;
  const Game *game_{nullptr};
  std::vector<unsigned> selected_;
};
