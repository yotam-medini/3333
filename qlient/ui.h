#pragma once
#include <memory>
#include <functional>
#include <QVariant>
#include "common.h"

class QVariant;
class UIImpl;

class UI {
 public:
  // using vmap_t = QVariantMap<QString, QVariant>;
  UI();
  ~UI();
  void SetClubRefresh(std::function<void(void)> f);
  void SetNewTableFunc(new_table_func_t f);
  void NewTable(const QVariantMap &result_map);
 private:
 std::unique_ptr<UIImpl> impl_{nullptr};
};
