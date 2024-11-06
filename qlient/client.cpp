#include "client.h"
#include <iostream>
#include <QString>
#include <unordered_set>
#include <fmt/core.h>
#include <QDebug>
#include <QtCore/qjsondocument.h>
#include <QJsonArray>
#include <QUrl>
#include <QtWebSockets/QWebSocket>
#include "../server/cs_consts.h"

#include "ui.h"
#include "game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
std::string getHostName() {
    // Access JavaScript's `window.location.hostname` directly
    emscripten::val window = emscripten::val::global("window");
    emscripten::val location = window["location"];
    std::string hostname = location["hostname"].as<std::string>();
    return hostname;
}
#endif

Client::Client(UI& ui, Game &game, QObject *parent) :
    QObject(parent),
    ui_{ui},
    game_{game} {
  using namespace Qt::Literals::StringLiterals;
  connect(&ws_, &QWebSocket::connected, this, &Client::OnConnected);
  connect(&ws_, &QWebSocket::disconnected, this, &Client::Closed);
  const std::string localhost{"localhost"};
  const std::string host_name =
#ifdef __EMSCRIPTEN__
    getHostName();
#else
    localhost;
#endif
  qDebug() << fmt::format("host_name={}", host_name);
  int port = 9090;
  QUrl url;
  QString scheme(host_name == localhost ? u"ws"_s : u"wss"_s);
  url.setScheme(scheme);
  url.setHost(QString::fromStdString(host_name));
  if (host_name == localhost) {
    url.setPort(port);
  } else {
    url.setPath("/sg");
  }
  qDebug() << fmt::format("Calling QWebSocket::open({})",
    url.toDisplayString().toStdString());
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
        break;
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

int Client::Try3(const std::vector<unsigned> &active_3cards) {
  int rc = 0;
  std::string command = fmt::format("{} {} {}",
    S3333_C2S_TRY3, game_.gstate_, fmt::join(active_3cards, " "));
  ws_.sendTextMessage(QString::fromStdString(command));
  return rc;
}

static bool IsSet(unsigned c0, unsigned c1, unsigned c2) {
  bool good = true;
  std::vector<unsigned> cards3{c0, c1, c2};
  for (size_t d = 0; good && (d < 4); ++d) {
    std::unordered_set<unsigned> dset;
    for (size_t ci = 0; ci < 3; ++ci) {
      unsigned m = cards3[ci] % 3;
      cards3[ci] = cards3[ci]/3;
      dset.insert(m);
    }
    good = (dset.size() != 2);
  }
  return good;
}

void Client::SetGameState(const QVariantMap &result_map) {
  game_.active_ = (result_map["gactive"].toInt() == 1);
  game_.tstate_ = result_map["tstate"].toInt();
  game_.gstate_ = result_map["gstate"].toInt();

  QJsonArray a = result_map["active"].toJsonArray();
  game_.cards_active_.clear();
  game_.cards_active_.reserve(a.size());
  for (auto iter = a.cbegin(); iter != a.cend(); ++iter) {
    game_.cards_active_.push_back(iter->toInt());
  }
  game_.deck_ = result_map["deck"].toInt();
  qDebug() << fmt::format("cards_ative_={}",
    fmt::join(game_.cards_active_, ", "));

  QVariantMap m = result_map["players"].toMap();
  game_.players_.clear();
  game_.players_.reserve(m.size());
  for (auto iter = m.cbegin(); iter != m.cend(); ++iter) {
    Player player;
    const QVariantMap player_map = iter->toMap();
    player.name_ = player_map["name"].toString().toStdString();
    const QJsonArray numbers = player_map["numbers"].toJsonArray();
    player.set_calls_good_ = numbers[0].toInt();
    player.set_calls_bad_  = numbers[1].toInt();
    player.add3_good_      = numbers[2].toInt();
    player.add3_bad_       = numbers[3].toInt();
    player.no_more_good_   = numbers[4].toInt();
    player.no_more_bad_    = numbers[5].toInt();
    player.tcreated_ = player_map["tcreated"].toInt();
    player.taction_ = player_map["taction"].toInt();
    game_.players_.push_back(player);
  }
                         
  static constexpr bool cheat = true;
  if (cheat) {
    qDebug() << "== Cheat ==";
    const std::vector<unsigned> &ca = game_.cards_active_;
    const size_t na = ca.size();
    for (size_t i = 0; i < na; ++i) {
      for (size_t j = i + 1; j < na; ++j) {
        for (size_t k = j + 1; k < na; ++k) {
          if (IsSet(ca[i], ca[j], ca[k])) {
            qDebug() << fmt::format("cheat: {} {} {}", i, j, k);
          }
        }
      }
    }
  }
}
