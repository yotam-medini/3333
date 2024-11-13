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

  QLabel *label;
  for (size_t ti = 0; ti < tables_info_.size(); ++ti) {
    const auto &table_info = tables_info_[ti];
      label = NewLabel(table_info.name_, this);
      tables_table_->addWidget(label, ti + 1, 0);
      label = NewLabel(fmt::format("{}", table_info.num_players_), this);
      tables_table_->addWidget(label, ti + 1, 1);
      label = NewLabel(ymdhms(table_info.tcreated_), this);
      tables_table_->addWidget(label, ti + 1, 2);
      label = NewLabel(ymdhms(table_info.taction_), this);
      tables_table_->addWidget(label, ti + 1, 3);
  }
}

void Club::SetRefresh(std::function<void(void)> f) {
  QObject::connect(butt_referesh_, &QPushButton::clicked, f);
}

#if 0
void Club::SetTablesInfo(const std::vector<TableInfo> &tables_info) {
  tables_info_ = tables_info;
  FillTablesTable();
}
#endif

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

Club::TitleSort::TitleSort(const std::string &text, QWidget *parent) {
  container_ = new QWidget(parent);
  container_->setStyleSheet("background-color: #eee;");
  QCommonStyle style;
  button_ascend_ = new QPushButton(parent);
  button_ascend_->setIcon(style.standardIcon(QStyle::SP_ArrowUp));
  button_descend_ = new QPushButton(parent);
  button_descend_->setIcon(style.standardIcon(QStyle::SP_ArrowDown));
  auto layout = new QHBoxLayout();
  layout->addWidget(new QLabel(QString::fromStdString(text), parent), 1);
  layout->addWidget(button_ascend_);
  layout->addWidget(button_descend_);
  container_->setLayout(layout);
}
