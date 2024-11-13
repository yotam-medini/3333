#pragma once

#include <string>
#include "dlgtbl.h"
#include "common.h"

class QLineEdit;

class JoinTable : public DialogTableBase {
 public:
  JoinTable(QWidget *parent, join_table_func_t f);
  std::string GetPlayerPassword() const;
 private:
  const join_table_func_t join_table_func_;
  QLineEdit *player_name_{nullptr};
  QLineEdit *player_password_{nullptr};
};
