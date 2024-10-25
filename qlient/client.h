#pragma once

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QString>

class UI;
class Game;

class Client : public QObject {
  Q_OBJECT
 public:
  explicit Client(UI& ui, Game& game, const QUrl &url, QObject *parent=nullptr);
  int ClubRefresh();
  int NewTable(
    const std::string &table_name,
    const std::string &table_password,
    const std::string &owner_password);
  int NewGame();
 Q_SIGNALS:
  void Closed();
 private Q_SLOTS:
  void OnConnected();
  void OnReceived(QString message);
 private :
  UI &ui_;
  Game &game_;
  QWebSocket ws_;
};

