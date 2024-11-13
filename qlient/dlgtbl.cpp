#include "dlgtbl.h"
#include <QLineEdit>

DialogTableBase::DialogTableBase(QWidget *parent) :
  QDialog(parent) {
}

std::string DialogTableBase::GetTableName() const {
  return table_name_->text().toStdString();
}

std::string DialogTableBase::GetTablePassword() const {
  return table_password_->text().toStdString();
}

