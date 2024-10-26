#include "table.h"
#include <fmt/core.h>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

class DrawArea : public QWidget {
 public:
  DrawArea(QWidget *parent) : QWidget(parent) {}
 protected:
  void paintEvent(QPaintEvent *event) override {
    int w = width();
    int h = height();
    qDebug() << fmt::format("DrawArea::paintEvent w={}, h={}", w, h);
    QPainter ell(this);
    ell.setPen(Qt::darkGreen);
    ell.drawEllipse(QRect(w/8, h/8, w/2, h/2)); 
  }
};

Table::Table(QWidget *parent) :
    QWidget(parent) {
  setStyleSheet("background-color:#392;");
  QVBoxLayout *vlayout = new QVBoxLayout(parent);
  QHBoxLayout *hlayout = new QHBoxLayout(parent);
  name_at_name_ = std::make_unique<QLabel>("<b>?@?</b>", this);
  hlayout->addWidget(name_at_name_.get());
  butt_new_game = std::make_unique<QPushButton>("New Game", this);
  connect(butt_new_game.get(), &QPushButton::clicked, [this]() { this->new_game_func_(); });
  hlayout->addWidget(butt_new_game.get());
  vlayout->addLayout(hlayout);
  draw_area_ = std::make_unique<DrawArea>(this);
  draw_area_->setStyleSheet("background-color:#9ab;");
  vlayout->addWidget(draw_area_.get());
  setLayout(vlayout);
}

Table::~Table() {
}

void Table::NewTable(const QVariantMap &result_map) {
  qDebug() << "Table::NewTable";
  std::string name = result_map["table_name"].toString().toStdString();
  qDebug() << fmt::format("name={}", name);
  std::string t = fmt::format("<b>{}@{}</b>", name, name);
  name_at_name_->setText(QString::fromStdString(t));
}

void Table::DrawGame(const Game& game) {
   //  setBackgroundRole(QPalette::Base);
   // setAutoFillBackground(true);
  
  int w = draw_area_->width();
  int h = draw_area_->width();
  draw_area_->update();
  qDebug() << fmt::format("Table::DrawGame w={}, h={}", w, h);
#if 0
    QPainterPath rectPath;
    rectPath.moveTo(w/4, h/4.0);
    rectPath.lineTo(3*w/4, h/4);
    rectPath.lineTo(3*w/4, 3*h/4);
    rectPath.lineTo(w/4, h/2);
    rectPath.closeSubpath();
  QPainter ell(this);
  ell.setPen(Qt::darkGreen);
  ell.drawEllipse(QRect(w/8, h/8, w/2, h/2)); 
#endif

}
