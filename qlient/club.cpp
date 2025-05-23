#include "club.h"
#include <string>
#include <fmt/core.h>
#include <QCommonStyle>
#include <QDebug>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include "jointbl.h"
#include "newtbl.h"
#include "utils.h"

Club::Club(QWidget *parent) : QWidget(parent) {
  qDebug("Club::Club");
  setStyleSheet("background-color:#ddd;");
  QVBoxLayout *vlayout = new QVBoxLayout();
  QHBoxLayout *hlayout = new QHBoxLayout();
  // QHBoxLayout *hlayout = new QHBoxLayout();
  butt_referesh_ = new QPushButton("Refresh", this);
  butt_new_table_ = new QPushButton("New Table", this);
  QObject::connect(butt_new_table_, &QPushButton::clicked,
    this, &Club::OpenNewTableDialog);
  hlayout->addWidget(new QWidget(this), 2);
  hlayout->addWidget(butt_referesh_);
  hlayout->addWidget(new QWidget(this), 1);
  hlayout->addWidget(butt_new_table_);
  hlayout->addWidget(new QWidget(this), 2);
  hlayout->addWidget(new QLabel("or Press name to join", this));
  hlayout->addWidget(new QWidget(this), 2);
  QWidget *bar = new QWidget(this);
  bar->setLayout(hlayout);
  vlayout->addWidget(bar);
  tables_table_ = new QGridLayout(this);

  QScrollArea *scrollArea = new QScrollArea;
  ttwrap_ = new QWidget(this);

  // Set the background color of the layout
  // QPalette palette = ttwrap_->palette();
  // palette.setColor(QPalette::Window, Qt::lightGray);
  // ttwrap_->setPalette(palette);
  ttwrap_->setStyleSheet("background-color: #234;");
  FillTablesTable();
 
  ttwrap_->setLayout(tables_table_);
  scrollArea->setWidget(ttwrap_);
  scrollArea->setWidgetResizable(true);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  vlayout->addWidget(scrollArea, 1);

  setLayout(vlayout);
}

Club::~Club() {
  qDebug("Club::~Club");
}

static QLabel *NewLabel(const std::string& text, QWidget* parent) {
  QLabel *label = new QLabel(QString::fromStdString(text), parent);
  label->setStyleSheet("background-color: white;");
  return label;
}

void Club::FillTablesTable() {
  tables_table_->setSpacing(10); // Adjust spacing as needed
  auto w = ttwrap_->width();
  auto h = ttwrap_->height();
  qDebug() << fmt::format("FillTablesTable w={} h={}", w, h);

  // clear
  while (QLayoutItem *item = tables_table_->takeAt(0)) {
    if (QWidget *widget = item->widget()) {
      widget->deleteLater();
    }
    delete item;
  }

  TitleSort title_name{"Name", this};
  tables_table_->addWidget(title_name.container_, 0, 0);

  TitleSort title_players{"#Players", this};
  tables_table_->addWidget(title_players.container_, 0, 1);

  TitleSort title_created{"Created", this};
  tables_table_->addWidget(title_created.container_, 0, 2);

  TitleSort title_action{"Action", this};
  tables_table_->addWidget(title_action.container_, 0, 3);

  bool same_create_ymd = true;
  bool same_action_ymd = true;
  if (!tables_info_.empty()) {
    const std::string ymd_created{ymd(tables_info_[0].tcreated_)};
    const std::string ymd_action{ymd(tables_info_[0].taction_)};
    for (size_t i = 1; (i < tables_info_.size()) &&
        (same_create_ymd || same_action_ymd); ++i) {
      const TableInfo &ti = tables_info_[i];
      same_create_ymd = same_create_ymd && (ymd_created == ymd(ti.tcreated_));
      same_action_ymd = same_action_ymd && (ymd_action == ymd(ti.taction_));
    }
  }
  using time_func_t = std::string(*)(unsigned);
  time_func_t created_tfunc = (same_create_ymd ? &hms : &ymdhms);
  time_func_t action_tfunc = (same_action_ymd ? &hms : &ymdhms);
  QLabel *label;
  for (size_t ti = 0; ti < tables_info_.size(); ++ti) {
    const auto &table_info = tables_info_[ti];
      QPushButton *name_join =
        new QPushButton(QString::fromStdString(table_info.name_), this);
      name_join->setStyleSheet("background-color: #2e3;");
      QObject::connect(name_join, &QPushButton::clicked,
        [this, table_info]() {
          this->OpenJoinTableDialog(table_info.name_);
        });
      tables_table_->addWidget(name_join, ti + 1, 0);
      label = NewLabel(fmt::format("{}", table_info.num_players_), this);
      tables_table_->addWidget(label, ti + 1, 1);
      label = NewLabel((*created_tfunc)(table_info.tcreated_), this);
      tables_table_->addWidget(label, ti + 1, 2);
      label = NewLabel((*action_tfunc)(table_info.taction_), this);
      tables_table_->addWidget(label, ti + 1, 3);
  }
}

void Club::SetRefresh(std::function<void(void)> f) {
  QObject::connect(butt_referesh_, &QPushButton::clicked, f);
}

void Club::Update(const QVariantMap &result_map) {
  tables_info_.clear();
  for (auto iter = result_map.constKeyValueBegin();
       iter != result_map.constKeyValueEnd(); ++iter) {
    const std::string table_name = (iter->first).toStdString();
    const QVariantMap m = (iter->second).toMap();
    unsigned num_players = m["players"].toInt();
    unsigned tcreated = m["tcreated"].toInt();
    unsigned taction = m["taction"].toInt();
    tables_info_.push_back(
      TableInfo(table_name, num_players, tcreated, taction));
  }
  FillTablesTable();
}
void Club::OpenNewTableDialog() {
  qDebug("new NewTable");
  if (!new_table_dialog_) {
    qDebug() << fmt::format("curr sizes: {}x{}", width(), height());
    new_table_dialog_ = new NewTable(this, this->new_table_func_);
    connect(new_table_dialog_, &QDialog::finished,
      [this](int result) {
        qDebug() << fmt::format("NewTable resultt={}", result);
        if (result == QDialog::Accepted) {
           NewTable *dlg = this->new_table_dialog_;
           this->new_table_func_(
             dlg->GetTableName(),
             dlg->GetTablePassword(),
             dlg->GetOwnerPassword());
        }
      });
  }
  new_table_dialog_->open();
}

void Club::OpenJoinTableDialog(const std::string &table_name) {
  qDebug("new Join Table");
  if (!join_table_dialog_) {
    qDebug() << fmt::format("curr sizes: {}x{}", width(), height());
    join_table_dialog_ = new JoinTable(this, this->join_table_func_);
    connect(join_table_dialog_, &QDialog::finished,
      [this, table_name](int result) {
        qDebug() << fmt::format("JoinTable resultt={}", result);
        if (result == QDialog::Accepted) {
           JoinTable *dlg = this->join_table_dialog_;
           this->join_table_func_(
             table_name,
             dlg->GetPlayerName(),
             dlg->GetTablePassword(),
             dlg->GetPlayerPassword());
        }
      });
  }
  join_table_dialog_->SetTableName(table_name);
  join_table_dialog_->open();
}

Club::TitleSort::TitleSort(const std::string &text, QWidget *parent) {
  container_ = new QWidget(parent);
  container_->setStyleSheet("background-color: #eee;");
  QCommonStyle style;
  auto vlayout = new QVBoxLayout();
  const std::string btextb = fmt::format("<b>{}</b>", text);
  vlayout->addWidget(new QLabel(QString::fromStdString(btextb), parent), 1);

  QWidget *buttons_box = new QWidget(parent);
  auto hlayout = new QHBoxLayout();
  button_ascend_ = new QPushButton(parent);
  button_ascend_->setIcon(style.standardIcon(QStyle::SP_ArrowUp));
  button_descend_ = new QPushButton(parent);
  button_descend_->setIcon(style.standardIcon(QStyle::SP_ArrowDown));
  hlayout->addWidget(button_ascend_);
  hlayout->addWidget(button_descend_);
  buttons_box->setLayout(hlayout);
  vlayout->addWidget(buttons_box);

  container_->setLayout(vlayout);
}
