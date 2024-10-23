#pragma once

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QString>

class Client : public QObject {
  Q_OBJECT
 public:
  explicit Client(const QUrl &url, QObject *parent=nullptr);
  int ClubRefresh();
 Q_SIGNALS:
  void Closed();
 private Q_SLOTS:
  void OnConnected();
  void OnTextMessageReceived(QString message);
 private :
  QWebSocket ws_;
};

