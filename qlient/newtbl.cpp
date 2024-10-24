#include "newtbl.h"
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>

NewTable::NewTable(QWidget *parent, new_table_func_t f) :
  QDialog(parent),
  new_table_func_(f) {
  qDebug() << "NewTable::NewTable\n";
  QVBoxLayout *layout = new QVBoxLayout(this);
  QLabel *title = new QLabel("Open New Table", this);
  layout->addWidget(title);
  QGridLayout *grid = new QGridLayout;
  grid->addWidget(new QLabel("Table Name", this), 0, 0);
  table_name_ = new QTextEdit();
  grid->addWidget(table_name_);
}
  
