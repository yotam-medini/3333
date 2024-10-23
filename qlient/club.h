#pragma once
#include <functional>
#include <memory>
#include <QPushButton>
#include <QWidget>

class Club : public QWidget {
 public:
  Club();
  ~Club();
  void SetRefresh(std::function<void(void)> f);
 private:
  std::unique_ptr<QPushButton> butt_referesh_;
  std::unique_ptr<QPushButton> butt_new_table;
};