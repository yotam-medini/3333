#pragma once
#include <memory>
#include <functional>
#include <vector>
#include <QVariant>
#include "common.h"

class QVariant;
class UIImpl;
class Game;

class UI {
 public:
  // using vmap_t = QVariantMap<QString, QVariant>;
  UI(Game &game);
  ~UI();
  void SetClubRefresh(std::function<void(void)> f);
  void SetNewTableFunc(new_table_func_t f);
  void NewTable(const QVariantMap &result_map);
  void SetNewGameFunc(std::function<int(void)> f);
  void SetTry3Func(std::function<int(const std::vector<unsigned>&)> f);
  void DrawGame();
 private:
  std::unique_ptr<UIImpl> impl_{nullptr};
};
