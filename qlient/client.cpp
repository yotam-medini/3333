#include "client.h"
#include <iostream>
#include <fmt/core.h>
#include <QDebug>
#include <QtCore/qjsondocument.h>
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
    QJsonDocument jd = QJsonDocument::fromJson(message.toUtf8());
  int rc = jd["rc"].toInt(-1);
  if (rc != 0) {
    qDebug() << QString::fromStdString(fmt::format("OnReceived: Failed: rc={}", rc));
  } else {
    int cmd = jd["cmd"].toInt(-1);
    QVariant result = jd["result"].toVariant();
    qDebug() << fmt::format("OnReceived: rc={}, result typeid={}",
      rc, result.typeId());
    if (result.typeId() == QMetaType::QVariantMap) {
      auto result_map = result.toMap();
      qDebug() << fmt::format("#(result)={}", result_map.size());
    }
  }    
  
}

int Client::ClubRefresh() {
  qDebug() << "Client::ClubRefresh\n";
  ws_.sendTextMessage(QString::fromStdString(S3333_C2S_TBLS));
  return 0;
}
