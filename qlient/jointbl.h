#pragma once

#include <string>
#include "dlgtbl.h"
#include "common.h"

class QLabel;
class QLineEdit;

class JoinTable : public DialogTableBase {
 public:
  JoinTable(QWidget *parent, join_table_func_t f);
  void SetTableName(const std::string &table_name);
  std::string GetPlayerName() const;
  std::string GetPlayerPassword() const;
 private:
  const join_table_func_t join_table_func_;
  QLabel *title_{nullptr};
  QLineEdit *player_name_{nullptr};
  QLineEdit *player_password_{nullptr};
};
