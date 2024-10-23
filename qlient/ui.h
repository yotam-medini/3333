#pragma once
#include <memory>

class UIImpl;

class UI {
 public:
  UI();
  ~UI();
  int run();
 private:
 std::unique_ptr<UIImpl> impl_{nullptr};
};
