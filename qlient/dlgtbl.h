#pragma once

#include <string>
#include <QDialog>

class QLineEdit;

class DialogTableBase : public QDialog {
 public:
  DialogTableBase(QWidget *parent);
  std::string GetTablePassword() const;
 protected:
  QLineEdit *table_password_{nullptr};
};
