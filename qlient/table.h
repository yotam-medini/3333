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
  void SetGame(const Game& game);
  const Game *GetGame() const { return game_; }
 private:
  void UpdateSelected();
  QLabel *name_at_name_{nullptr};
  QPushButton *butt_new_game_{nullptr};
  QPushButton *butt_add3_nomore_{nullptr};
  QLabel *picked_{nullptr};
  QLabel *status_summary_{nullptr};
  DrawArea *draw_area_{nullptr};
  std::function<int(void)> new_game_func_;
  const Game *game_{nullptr};
  std::vector<unsigned> selected_;
};
