#include "newtbl.h"
#include <fmt/core.h>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>
#include <QLineEdit>

NewTable::NewTable(QWidget *parent, new_table_func_t f) :
    QDialog(parent),
    new_table_func_(f) {
  setWindowTitle("New Table");
  auto parent_w = parent->width();
  auto parent_h = parent->height();
  auto w = (3*parent_w)/8;
  auto h = (3*parent_h)/8;
  qDebug() << fmt::format("NewTable::NewTable: {}x{}", w, h);
  resize(w, h);
  QVBoxLayout *layout = new QVBoxLayout(this);
  QLabel *title = new QLabel("<b>Open New Table</b>", this);
  layout->addWidget(title, 0, Qt::AlignHCenter);
  QGridLayout *grid = new QGridLayout;
  auto alphanum = QRegularExpression("[A-Za-z0-9_]+");

  grid->addWidget(new QLabel("Table Name", this), 0, 0);
  table_name_ = new QLineEdit();
  table_name_->setValidator(new QRegularExpressionValidator(alphanum));
  grid->addWidget(table_name_, 0, 1);


  grid->addWidget(new QLabel("Table Password", this), 1, 0);
  table_password_ = new QLineEdit();
  table_password_->setValidator(new QRegularExpressionValidator(alphanum));
  grid->addWidget(table_password_, 1, 1);

  grid->addWidget(new QLabel("Owner Password", this), 2, 0);
  owner_password_ = new QLineEdit();
  owner_password_->setValidator(new QRegularExpressionValidator(alphanum));
  grid->addWidget(owner_password_, 2, 1);

  layout->addLayout(grid);
  QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
  buttonBox->setStandardButtons(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  connect(table_name_, &QLineEdit::textChanged, [this, buttonBox]() {
    const QString table_name = this->table_name_->text();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!table_name.isEmpty());
  });
  connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
    this, &NewTable::accept);
  connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
    this, &NewTable::reject);
  
  layout->addWidget(buttonBox);
}
  
