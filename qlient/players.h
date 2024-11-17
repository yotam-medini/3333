#pragma once
#include <QWidget>

class QGridLayout;
class QLabel;
class Game;

class Players : public QWidget {
 public:
  Players(QWidget *parent);
  ~Players();
  void NewTable(const QVariantMap &result_map);
  void UpdatePlayers(const Game& game);
 private:
  void SetGridTitleRow();
  void ClearGridData();
  std::string table_name_;
  QLabel *title_;
  QGridLayout *grid_{nullptr};  
};
