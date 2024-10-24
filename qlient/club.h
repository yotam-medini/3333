#pragma once
#include <functional>
#include <memory>
#include <QWidget>
#include "common.h"

class QPushButton;
class NewTable;

class Club : public QWidget {
 public:
  Club();
  ~Club();
  void SetRefresh(std::function<void(void)> f);
  void setNewTableFunc(new_table_func_t f) { new_table_func_ = f; }
 private:
  void OpenNewTableDialog();
  std::unique_ptr<QPushButton> butt_referesh_;
  std::unique_ptr<QPushButton> butt_new_table_;
  std::unique_ptr<NewTable> new_table_dialog_;
  new_table_func_t new_table_func_;
};
