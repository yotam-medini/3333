#pragma once

#include <fmt/core.h>
#include <QDialog>
#include <QDebug>
#include <QTextEdit>
#include "common.h"

class QLineEdit;

class NewTable : public QDialog {
 public:
  NewTable(QWidget *parent, new_table_func_t f);
 private:
  const new_table_func_t new_table_func_;
  QLineEdit *table_name_{nullptr};
};
