#include "ui.h"
#include <memory>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class UIImpl {
 public:
  UIImpl() {
    window_ = std::make_unique<QWidget>();
    window_->setWindowTitle("Set-Game");
    window_->setStyleSheet("background-color:#1e2;");
    QVBoxLayout *layout = new QVBoxLayout(window_.get());
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
