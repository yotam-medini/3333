#pragma once

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QString>

class Client : public QObject {
  Q_OBJECT
 public:
  explicit Client(const QUrl &url, QObject *parent=nullptr);
  int ClubRefresh();
  int NewTable(
    const std::string &table_name,
    const std::string &table_password,
    const std::string &owner_password);

 Q_SIGNALS:
  void Closed();
 private Q_SLOTS:
  void OnConnected();
  void OnReceived(QString message);
 private :
  QWebSocket ws_;
};

