#include "club.h"
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "newtbl.h"

Club::Club(QWidget *parent) : QWidget(parent) {
  qDebug("Club::Club");
  setStyleSheet("background-color:#ddd;");
  QVBoxLayout *vlayout = new QVBoxLayout(this);
  QHBoxLayout *hlayout = new QHBoxLayout(this);
  butt_referesh_ = new QPushButton("Refresh", this);
  butt_new_table_ = new QPushButton("New Table", this);
  QObject::connect(butt_new_table_, &QPushButton::clicked,
    this, &Club::OpenNewTableDialog);
  hlayout->addWidget(butt_referesh_);
  hlayout->addWidget(butt_new_table_);
  vlayout->addLayout(hlayout);
  setLayout(vlayout);
}

Club::~Club() {
  qDebug("Club::~Club");
}

void Club::SetRefresh(std::function<void(void)> f) {
  QObject::connect(butt_referesh_, &QPushButton::clicked, f);
}

void Club::OpenNewTableDialog() {
  qDebug("new NewTable");
  if (!new_table_dialog_) {
    qDebug() << fmt::format("curr sizes: {}x{}", width(), height());
    new_table_dialog_ = new NewTable(this, this->new_table_func_);
    connect(new_table_dialog_, &QDialog::finished,
      [this](int result) {
        qDebug() << fmt::format("NewTable resultt={}", result);
        if (result == QDialog::Accepted) {
           NewTable *dlg = this->new_table_dialog_;
           this->new_table_func_(
             dlg->GetTableName(),
             dlg->GetTablePassword(),
             dlg->GetOwnerPassword());
        }
      });
  }
  new_table_dialog_->open();
}


