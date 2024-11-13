#pragma once
#include <functional>
#include <string>
#include <vector>
#include <QWidget>
#include "common.h"

class QGridLayout;
class QPushButton;
class QWidget;
class NewTable;

class Club : public QWidget {
 public:
  Club(QWidget *parent);
  ~Club();
  void SetRefresh(std::function<void(void)> f);
  // void SetTablesInfo(const std::vector<TableInfo> &tables_info);
  void Update(const QVariantMap &result_map);
  void SetNewTableFunc(new_table_func_t f) { new_table_func_ = f; }
  void SetJoinTableFunc(join_table_func_t f) { join_table_func_ = f; }
 private:
  class TableInfo {
   public:
    TableInfo(
      const std::string &name="",
      unsigned num_players=0,
      unsigned tcreated=0,
      unsigned taction=0) :
        name_{name},
        num_players_{num_players},
        tcreated_{tcreated},
        taction_{taction} {
      }
    std::string name_;
    unsigned num_players_{0};
    unsigned tcreated_{0};
    unsigned taction_{0};
  };
  class TitleSort {
   public:
    TitleSort(const std::string &, QWidget *parent);
    QWidget *container_{nullptr};
    QPushButton *button_ascend_{nullptr};
    QPushButton *button_descend_{nullptr};
  };
  void FillTablesTable();
  void OpenNewTableDialog();
  QPushButton *butt_referesh_{nullptr};
  QPushButton *butt_new_table_{nullptr};
  QWidget *ttwrap_{nullptr};
  QGridLayout *tables_table_{nullptr};
  NewTable *new_table_dialog_{nullptr};
  new_table_func_t new_table_func_;
  join_table_func_t join_table_func_;
  std::vector<TableInfo> tables_info_;
};
