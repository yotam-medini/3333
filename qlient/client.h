#pragma once

#include <vector>
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QString>

class UI;
class Game;

class Client : public QObject {
  Q_OBJECT
 public:
  explicit Client(
    UI& ui,
    Game& game,
    const std::string &force_url="",
    QObject *parent=nullptr);
  int ClubRefresh();
  int NewTable(
    const std::string &table_name,
    const std::string &table_password,
    const std::string &owner_password);
  int NewGame();
  int Add3NoMore();
  int Try3(const std::vector<unsigned> &active_3cards);
 Q_SIGNALS:
  void Closed();
 private Q_SLOTS:
  void OnConnected();
  void OnReceived(QString message);
 private :
  void SetGameState(const QVariantMap &result_map);
  void UpdateClub(const QVariantMap &result_map);
  UI &ui_;
  Game &game_;
  QWebSocket ws_;
};

