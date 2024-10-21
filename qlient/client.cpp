#include "client.h"
#include <iostream>

Client::Client(const QUrl &url, QObject *parent) :
    QObject(parent) {
  connect(&ws_, &QWebSocket::connected, this, &Client::OnConnected);
  connect(&ws_, &QWebSocket::disconnected, this, &Client::Closed);
  ws_.open(url);
}

void Client::OnConnected() {
  std::cout << "OnConnected\n";
}

void Client::OnTextMessageReceived(QString message) {
  std::cout << "OnTextMessageReceived: " << message.toStdString() << '\n';
}
