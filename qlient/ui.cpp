#include "ui.h"
#include <memory>
#include <QDebug>
#include <QPushButton>
// #include <QQuickWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

class UIImpl {
 public:
  UIImpl() {
    window_ = std::make_unique<QWidget>();
    window_->setWindowTitle("Set-Game");
    window_->setStyleSheet("background-color:#1e2;");
    QVBoxLayout *layout = new QVBoxLayout(window_.get());

    tab_ = std::make_unique<QTabWidget>();
    club_ = std::make_unique<QWidget>();
    club_idx_ = tab_->addTab(club_.get(), "Club");
    layout->addWidget(tab_.get());
    table_ = std::make_unique<QWidget>();
    table_idx_ = tab_->addTab(table_.get(), "Table");
    layout->addWidget(tab_.get());
    players_ = std::make_unique<QWidget>();
    players_idx_ = tab_->addTab(players_.get(), "Players");
    layout->addWidget(tab_.get());

    QPushButton *button1 = new QPushButton("Button 1");
    layout->addWidget(button1);
    QObject::connect(button1, &QPushButton::clicked, [](){
        qDebug("Button 1 clicked!");
    });
    window_->setLayout(layout);
    window_->resize(400, 300);
    window_->show();
  }
 private:
  std::unique_ptr<QWidget> window_;
  std::unique_ptr<QTabWidget> tab_;
  std::unique_ptr<QWidget> club_, table_, players_;
  int club_idx_, table_idx_, players_idx_;
};

UI::UI() {
  impl_ = std::make_unique<UIImpl>();
}

UI::~UI() {
  qDebug("UI::~UI");
}

int UI::run() {
  int rc = 0;
  return rc;
}
