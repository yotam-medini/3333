#pragma once

#include <string>
#include <QDialog>
#include "dlgtbl.h"
#include "common.h"

class QLineEdit;

class NewTable : public DialogTableBase {
 public:
  NewTable(QWidget *parent, new_table_func_t f);
  std::string GetOwnerPassword() const;
 private:
  const new_table_func_t new_table_func_;
  QLineEdit *owner_password_{nullptr};
};
