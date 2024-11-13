#include "jointbl.h"
#include <fmt/core.h>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>
#include <QLineEdit>

JoinTable::JoinTable(QWidget *parent, join_table_func_t f) :
    DialogTableBase(parent),
    join_table_func_(f) {
  setWindowTitle("Join Table");
  auto parent_w = parent->width();
  auto parent_h = parent->height();
  auto w = (3*parent_w)/8;
  auto h = (3*parent_h)/8;
  qDebug() << fmt::format("JoinTable::JoinTable: {}x{}", w, h);
  resize(w, h);
  QVBoxLayout *layout = new QVBoxLayout(this);
  title_ = new QLabel("<b>Join Table</b>", this);
  layout->addWidget(title_, 0, Qt::AlignHCenter);
  QGridLayout *grid = new QGridLayout;
  auto alphanum = QRegularExpression("[A-Za-z0-9_]+");

  grid->addWidget(new QLabel("Player Name", this), 0, 0);
  player_name_ = new QLineEdit(this);
  player_name_->setValidator(new QRegularExpressionValidator(alphanum));
  grid->addWidget(player_name_, 0, 1);

  grid->addWidget(new QLabel("Table Password", this), 1, 0);
  table_password_ = new QLineEdit(this);
  table_password_->setValidator(new QRegularExpressionValidator(alphanum));
  grid->addWidget(table_password_, 1, 1);

  grid->addWidget(new QLabel("Player Password", this), 2, 0);
  player_password_ = new QLineEdit(this);
  player_password_->setValidator(new QRegularExpressionValidator(alphanum));
  grid->addWidget(player_password_, 2, 1);

  layout->addLayout(grid);
  QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
  buttonBox->setStandardButtons(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  connect(player_name_, &QLineEdit::textChanged, [this, buttonBox]() {
    const QString table_name = this->player_name_->text();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!table_name.isEmpty());
  });
  connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
    this, &JoinTable::accept);
  connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
    this, &JoinTable::reject);
  
  layout->addWidget(buttonBox);
}

void JoinTable::SetTableName(const std::string &table_name) {
    std::string join_title = fmt::format("Join Table {}", table_name);
    setWindowTitle(QString::fromStdString(join_title));
    join_title = fmt::format("<b>{}</b>", join_title);
    title_->setText(QString::fromStdString(join_title));
}

std::string JoinTable::GetPlayerName() const {
  return player_name_->text().toStdString();
}

std::string JoinTable::GetPlayerPassword() const {
  return player_password_->text().toStdString();
}
