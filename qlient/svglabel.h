#pragma once

#include <QLabel>

class QByteArray;
class QSvgRenderer;

class SvgLabel : public QLabel {
  Q_OBJECT
 public:
  explicit SvgLabel(const QByteArray& data, QWidget *parent = nullptr);
  ~SvgLabel() override;
 protected:
  void paintEvent(QPaintEvent *event) override;
 private:
  QSvgRenderer *renderer_{nullptr};
};
