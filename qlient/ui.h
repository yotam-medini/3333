#pragma once
#include <memory>
#include <functional>

class UIImpl;

class UI {
 public:
  UI();
  ~UI();
  int run();
  void SetClubRefresh(std::function<void(void)> f);
 private:
 std::unique_ptr<UIImpl> impl_{nullptr};
};
