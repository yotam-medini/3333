#include "client.h"
#include <iostream>
#include <fmt/core.h>
#include <QDebug>
#include "../server/cs_consts.h"

Client::Client(const QUrl &url, QObject *parent) :
    QObject(parent) {
  connect(&ws_, &QWebSocket::connected, this, &Client::OnConnected);
  connect(&ws_, &QWebSocket::disconnected, this, &Client::Closed);
  ws_.open(url);
}

void Client::OnConnected() {
  std::cout << "OnConnected\n";
  connect(&ws_, &QWebSocket::textMessageReceived, this, &Client::OnReceived);
}

void Client::OnReceived(QString message) {
  std::cout << "OnTextMessageReceived: " << message.toStdString() << '\n';
  qDebug() << fmt::format("OnTextMessageReceived: {}\n",
    message.toStdString());
}

int Client::ClubRefresh() {
  qDebug() << "Client::ClubRefresh\n";
  ws_.sendTextMessage(QString::fromStdString(S3333_C2S_TBLS));
  return 0;
}
