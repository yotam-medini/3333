#pragma once
#include <functional>
#include <QWidget>
#include "common.h"

class QPushButton;
class NewTable;

class Club : public QWidget {
 public:
  Club(QWidget *parent);
  ~Club();
  void SetRefresh(std::function<void(void)> f);
  void setNewTableFunc(new_table_func_t f) { new_table_func_ = f; }
 private:
  void OpenNewTableDialog();
  QPushButton *butt_referesh_{nullptr};
  QPushButton *butt_new_table_{nullptr};
  NewTable *new_table_dialog_{nullptr};
  new_table_func_t new_table_func_;
};
