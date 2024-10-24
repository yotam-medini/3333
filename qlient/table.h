#pragma once

#include <memory>
#include <QWidget>
#include "common.h"

class QLabel;
class QPushButton;
// class QVariantMap;
class QWidget;

class Table : public QWidget {
 public:
  Table(QWidget *parent);
  ~Table();
  void SetName(const std::string &s);
  void NewTable(const QVariantMap &result_map);
 private:
  std::unique_ptr<QLabel> name_at_name_;
  std::unique_ptr<QPushButton> butt_new_game;
  std::unique_ptr<QWidget> draw_area_;
};
