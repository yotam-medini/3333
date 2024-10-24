#pragma once
#include <memory>
#include <functional>
#include "common.h"

class UIImpl;

class UI {
 public:
  UI();
  ~UI();
  void SetClubRefresh(std::function<void(void)> f);
  void setNewTableFunc(new_table_func_t f);
 private:
 std::unique_ptr<UIImpl> impl_{nullptr};
};
