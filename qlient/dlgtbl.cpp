#include "dlgtbl.h"
#include <QLineEdit>

DialogTableBase::DialogTableBase(QWidget *parent) :
  QDialog(parent) {
}

std::string DialogTableBase::GetTablePassword() const {
  return table_password_->text().toStdString();
}

