#pragma once

#include <string>
#include <QDialog>

class QLineEdit;

class DialogTableBase : public QDialog {
 public:
  DialogTableBase(QWidget *parent);
  std::string GetTableName() const;
  std::string GetTablePassword() const;
 protected:
  QLineEdit *table_name_{nullptr};
  QLineEdit *table_password_{nullptr};
};
