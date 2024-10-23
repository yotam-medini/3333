#include "club.h"
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

Club::Club() {
  qDebug("Club::Club");
  setStyleSheet("background-color:#ddd;");
  QVBoxLayout *vlayout = new QVBoxLayout(this);
  QHBoxLayout *hlayout = new QHBoxLayout();
  butt_referesh_ = std::make_unique<QPushButton>("Refresh");
  butt_new_table = std::make_unique<QPushButton>("New Table");
  hlayout->addWidget(butt_referesh_.get());
  hlayout->addWidget(butt_new_table.get());
  vlayout->addLayout(hlayout);
  setLayout(vlayout);
}

Club::~Club() {
  qDebug("Club::~Club");
}

