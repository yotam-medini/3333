#pragma once

#include <functional>
#include <memory>
#include <QWidget>
#include "common.h"

class QLabel;
class QPushButton;
class QWidget;

class Game;

class Table : public QWidget {
 public:
  Table(QWidget *parent);
  ~Table();
  void SetName(const std::string &s);
  void NewTable(const QVariantMap &result_map);
  void SetNewGameFunc(std::function<int(void)> f) { new_game_func_ = f; }
  void DrawGame(const Game& game);
 private:
  std::unique_ptr<QLabel> name_at_name_;
  std::unique_ptr<QPushButton> butt_new_game;
  std::unique_ptr<QWidget> draw_area_;
  std::function<int(void)> new_game_func_;
};
