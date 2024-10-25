#include "ui.h"
#include <memory>
#include <QDebug>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "club.h"
#include "table.h"

class UIImpl {
 public:
  UIImpl(Game &game) :
      game_{game} {
    window_ = std::make_unique<QWidget>();
    window_->setWindowTitle("Set-Game");
    window_->setStyleSheet("background-color:#1b2;");
    QVBoxLayout *layout = new QVBoxLayout(window_.get());

    tab_ = std::make_unique<QTabWidget>();
    club_ = std::make_unique<Club>();
    club_idx_ = tab_->addTab(club_.get(), "Club");
    layout->addWidget(tab_.get());
    table_ = std::make_unique<Table>(window_.get());
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
    window_->resize(800, 600);
    window_->show();
  }
  void SetClubRefresh(std::function<void(void)> f) {
    club_->SetRefresh(f);
  }
  void setNewTableFunc(new_table_func_t f) {
    club_->setNewTableFunc(f);
  }
  void NewTable(const QVariantMap &result_map) {
    tab_->setCurrentIndex(table_idx_);
    table_->NewTable(result_map);
  }
  void SetNewGameFunc(std::function<int(void)> f) {
    table_->SetNewGameFunc(f);
  }
 private:
  Game &game_; 
  std::unique_ptr<QWidget> window_;
  std::unique_ptr<QTabWidget> tab_;
  std::unique_ptr<Club> club_;
  std::unique_ptr<Table> table_;
  std::unique_ptr<QWidget> players_;
  int club_idx_, table_idx_, players_idx_;
};

UI::UI(Game &game) {
  impl_ = std::make_unique<UIImpl>(game);
}

UI::~UI() {
  qDebug("UI::~UI");
}

void UI::SetClubRefresh(std::function<void(void)> f) {
  impl_->SetClubRefresh(f);
}

void UI::SetNewTableFunc(new_table_func_t f) {
  impl_->setNewTableFunc(f);
}

void UI::NewTable(const QVariantMap &result_map) {
  impl_->NewTable(result_map);
}

void UI::SetNewGameFunc(std::function<int(void)> f) {
  impl_->SetNewGameFunc(f);
}
