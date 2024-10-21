#pragma once

#include <QtWebSockets/QWebSocket>
#include <QtCore/QString>

class Client : public QObject {
  Q_OBJECT
 public:
  explicit Client(const QUrl &url, QObject *parent=nullptr);
 Q_SIGNALS:
  void closed();
 private Q_SLOTS:
  void OnConnected();
  void OnTextMessageReceived(QString message);
 private :
  QWebSocket ws_;
};

