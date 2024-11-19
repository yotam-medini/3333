#include "svglabel.h"
#include <fmt/core.h>
#include <QDebug>
#include <QByteArray>
#include <QPainter>
#include <QSvgRenderer>

SvgLabel::SvgLabel(const QByteArray& data, QWidget *parent) :
  QLabel(parent),
  renderer_{new QSvgRenderer(data, this)} {
  setStyleSheet("background-color: white;");
  setMinimumSize(0x10, 0x10);
  auto policy = sizePolicy();
  qDebug() << fmt::format("SvgLabel strech={} {}",
    policy.horizontalStretch(), policy.verticalStretch());
#if 0
  policy.setHorizontalStretch(1);
  policy.setVerticalStretch(1);
 // setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
  policy.setVerticalPolicy(QSizePolicy::MinimumExpanding);
  setSizePolicy(policy);
#endif
  // renderer_->setAspectRatioMode(Qt::KeepAspectRatio);
}

SvgLabel::~SvgLabel() {
  delete renderer_;
}

void SvgLabel::paintEvent(QPaintEvent *event) {
  QLabel::paintEvent(event);
  QPainter painter(this);
  QSize labelSize = size();
  auto w = labelSize.width();
  auto h = labelSize.width();
  // qDebug() << fmt::format("SvgLabel w={} h={}\n", w, h);
  QRectF targetRect(0, 0, w, h);
  renderer_->render(&painter, targetRect);
}
