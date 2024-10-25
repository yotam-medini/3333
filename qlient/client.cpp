#include "client.h"
#include <iostream>
#include <fmt/core.h>
#include <QDebug>
#include <QtCore/qjsondocument.h>
#include <QJsonArray>
#include "../server/cs_consts.h"

#include "ui.h"
#include "game.h"

Client::Client(UI& ui, Game &game, const QUrl &url, QObject *parent) :
    QObject(parent),
    ui_{ui},
    game_{game} {
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
      switch (cmd) {
       case E3333_S2C_TBLS:
        qDebug() << "E3333_S2C_TBLS not yet supported";
        break;
       case E3333_S2C_NTBL:
        ui_.NewTable(result_map);
        break;
       case E3333_S2C_GSTATE:
        SetGameState(result_map);
        ui_.DrawGame();
       default:
         qDebug() << fmt::format("Unsupported E3333_S2C_xxx {} command", cmd);
      };
    } else {
      qDebug() << "result not a QVariantMap";
    }
  }    
  
}

int Client::ClubRefresh() {
  qDebug() << "Client::ClubRefresh\n";
  ws_.sendTextMessage(QString::fromStdString(S3333_C2S_TBLS));
  return 0;
}

int Client::NewTable(
    const std::string &table_name,
    const std::string &table_password,
    const std::string &owner_password) {
  int rc = 0;
  qDebug() << fmt::format("Client::NewTable table_name={} "
    " table_password={} owner_password={}",
    table_name, table_password, owner_password);
  const unsigned flags =
    (table_password.empty() ? 0 : 1) +
    (owner_password.empty() ? 0 : 2);
  std::string command = fmt::format("{} {} {} {} {}",
    S3333_C2S_NTBL, table_name, flags, table_password, owner_password);
  qDebug() << fmt::format("command= {}", command);
  ws_.sendTextMessage(QString::fromStdString(command));
  return rc;
}

int Client::NewGame() {
  int rc = 0;
  qDebug() << "Client::NewGame";
  ws_.sendTextMessage(QString::fromStdString(S3333_C2S_GNEW));
  return rc;
}

void Client::SetGameState(const QVariantMap &result_map) {
  game_.active_ = (result_map["gactive"].toInt() == 1);
  game_.tstate_ = result_map["tstate"].toInt();
  game_.gstate_ = result_map["gstate"].toInt();

  const QJsonArray a =  result_map["active"].toJsonArray();
  game_.cards_active_.clear();
  game_.cards_active_.reserve(a.size());
  for (auto iter = a.cbegin(); iter != a.cend(); ++iter) {
    game_.cards_active_.push_back(iter->toInt());
  }
  qDebug() << fmt::format("cards_ative_={}",
    fmt::join(game_.cards_active_, ", "));
}

