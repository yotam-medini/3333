#include "club.h"
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "newtbl.h"

Club::Club() {
  qDebug("Club::Club");
  setStyleSheet("background-color:#ddd;");
  QVBoxLayout *vlayout = new QVBoxLayout(this);
  QHBoxLayout *hlayout = new QHBoxLayout();
  butt_referesh_ = std::make_unique<QPushButton>("Refresh");
  butt_new_table_ = std::make_unique<QPushButton>("New Table");
  QObject::connect(butt_new_table_.get(), &QPushButton::clicked,
    this, &Club::OpenNewTableDialog);
  hlayout->addWidget(butt_referesh_.get());
  hlayout->addWidget(butt_new_table_.get());
  vlayout->addLayout(hlayout);
  setLayout(vlayout);
}

Club::~Club() {
  qDebug("Club::~Club");
}

void Club::SetRefresh(std::function<void(void)> f) {
  QObject::connect(butt_referesh_.get(), &QPushButton::clicked, f);
}

void Club::OpenNewTableDialog() {
  qDebug("new NewTable");
  if (!new_table_dialog_) {
    qDebug() << fmt::format("curr sizes: {}x{}", width(), height());
    new_table_dialog_ = std::make_unique<NewTable>(
      this, this->new_table_func_);
    connect(new_table_dialog_.get(), &QDialog::finished,
      [this](int result) {
        qDebug() << fmt::format("NewTable resultt={}", result);
        if (result == QDialog::Accepted) {
           NewTable *dlg = this->new_table_dialog_.get();
           this->new_table_func_(
             dlg->GetTableName(),
             dlg->GetTablePassword(),
             dlg->GetOwnerPassword());
        }
      });
  }
  new_table_dialog_->open();
}


