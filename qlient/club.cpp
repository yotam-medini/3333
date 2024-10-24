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
    [this]() {
      qDebug("new NewTable");
      NewTable *dlg = new NewTable(this, this->new_table_func_);
      auto ret = dlg->exec();
      qDebug() << fmt::format("NewTable sizeof(ret)={}", sizeof(ret));
    });
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

