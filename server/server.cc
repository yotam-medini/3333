#include "server.h"
#include <charconv>
#include <cstdint>
#include <iostream>
#include <functional>
#include <fstream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <fmt/core.h>

#include "net.h"
#include "http_session.h"
#include "utils.h"

#include "player.h"
#include "table.h"
#include "ws_session.h"

// TO be moved to common constants with Javascript code
static const std::string S3333_C2S_TBLS = "tbls";
static const std::string S3333_C2S_NTBL = "ntbl";
static const std::string S3333_C2S_GNEW = "gnew";
static const std::string S3333_C2S_TRY3 = "try3";
static const unsigned E3333_S2C_TBLS = 0;
static const unsigned E3333_S2C_NTBL = 1;
static const unsigned E3333_S2C_GSTATE = 2;
static const unsigned E3333_S2C_N = 3;



class WebSocketSession;
using notify_ws_t = std::function<void(WebSocketSession*)>;
using report_message_t =
  std::function<void(WebSocketSession*, const std::string&)>;

class NetServer {
 public:
  NetServer(
    const std::string &host,
    uint16_t port,
    notify_ws_t notify_ws_add,
    notify_ws_t notify_ws_delete,
    report_message_t report_message) :
    host_{host},
    port_(port),
    acceptor_{ioc_},
    socket_{ioc_},
    notify_ws_add_{notify_ws_add},
    notify_ws_delete_{notify_ws_delete_},
    report_message_{report_message} {
  }
  void run() {
    const std::string fn = funcname();
    error_code ec;
    tcp::endpoint endpoint;
    auto address = net::ip::make_address(host_, ec);
    if (ec) {
      std::cerr << fn << " make_address failed, ec=" << ec << '\n';
    } else {
      endpoint = tcp::endpoint{address, port_};
    }
    if (!ec) {
      acceptor_.open(endpoint.protocol(), ec);
      if (ec) {
        std::cerr << fn << " acceptor.open failed, ec=" << ec << '\n';
      } else {
        acceptor_.set_option(net::socket_base::reuse_address(true));
        if (ec) {
          std::cerr << fn << " acceptor.set_option failed, ec=" << ec << '\n';
        }
      }
    }
    if (!ec) {
      acceptor_.bind(endpoint, ec);
      if (ec) {
        std::cerr << fn << " acceptor.bind failed, ec=" << ec << '\n';
      }
    }
    if (!ec) {
      acceptor_.listen(net::socket_base::max_listen_connections, ec);
      if (ec) {
        std::cerr << fn << "acceptor.listen failed, ec=" << ec << '\n';
      }
    }
    if (!ec) {
      // Start accepting a connection
      accept_next();
      ioc_.run();
    }
  }
 private:
  void accept_next() {
    acceptor_.async_accept(
      socket_,
      [this](error_code ec) {
        this->on_accept(ec);
      });
  }
  void on_accept(error_code ec) {
    if (ec) {
      std::cerr << "accept failedm ec=" << ec << '\n';
    } else {
      // Launch a new session for this connection
      HttpSession *hs = new HttpSession{
        std::move(socket_),
        // register_ws
        [this](HttpSession *dhs, WebSocketSession *ws) {
          hs_to_ws_[dhs] = ws;
        },
        // unregister http
        [this](HttpSession *dhp) {
          WebSocketSession *ws = hs_to_ws_[dhp];
          if (ws) { // should move to upper server
            notify_ws_delete_(ws);
          }
          this->hs_to_ws_.erase(dhp);
        },
        report_message_
      };
      hs_to_ws_[hs] = nullptr;
      hs->run();
    }
    accept_next();
  }
  const std::string host_;
  const uint16_t port_;
  net::io_context ioc_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  notify_ws_t notify_ws_add_;
  notify_ws_t notify_ws_delete_;
  report_message_t report_message_;
  std::unordered_map<HttpSession*, WebSocketSession*> hs_to_ws_;
};

Server::Server(
  const std::string &host,
  uint16_t port,
  const size_t max_tables,
  const size_t max_players,
  const unsigned expire_seconds,
  const std::string &pidfn,
  const unsigned debug_flags
) :
    max_tables_{max_tables},
    max_players_{max_players},
    expire_seconds_{expire_seconds},
    pidfn_{pidfn},
    debug_flags_{debug_flags},
    net_server_{
      new NetServer{
        host,
        port,
        [this](WebSocketSession *ws) -> void {
          this->ws_player_[ws] = nullptr;
        },
        [this](WebSocketSession *ws) -> void { this->ws_deleted(ws); },
        [this](WebSocketSession *ws, const std::string& message) -> void {
          this->ws_received_message(ws, message);
        }
      }
    } {
}

Server::~Server() {
  delete net_server_;
}

void Server::run() {
  std::ofstream(pidfn_) << getpid() << '\n';
  net_server_->run();
}

void Server::ws_deleted(WebSocketSession *ws) {
  auto iter = ws_player_.find(ws);
  ws_player_.erase(iter);
}

void Server::ws_received_message(
  WebSocketSession *ws,
  const std::string &message) {
  std::vector<std::string> cmd = ssplit(message);
  if (debug_flags_ & 0x1) {
    std::cerr << ymdhms() << ' ' << funcname() <<
      " message=" << message << '\n';
    std::cerr << fmt::format("{} {} message='{}', #(cmd)={}\n",
      ymdhms(), funcname(), message, cmd.size());
  }
  if (!cmd.empty()) {
    std::string err;
    if (cmd[0] == S3333_C2S_TBLS) {
      ws->send(server_to_client(E3333_S2C_TBLS, 0, tables_to_json()));
    } else if (cmd[0] == S3333_C2S_NTBL) {
      err = new_table(cmd, ws);
      if (err.empty()) {
        ws->send(server_to_client(E3333_S2C_NTBL, 0, R"("")"));
      }
    } else if (cmd[0] == S3333_C2S_GNEW) {
      Player *player = ws_player_[ws];
      Table *table = player->GetTable();
      table->NewGame();
      const std::string jts = GetTableStatusJson(table);
      for (auto &tplayer: table->GetPlayers()) {
        tplayer->GetWS()->send(server_to_client(E3333_S2C_GSTATE, 0, jts));
      }
    } else {
      err = fmt::format("Unsupported command='{}'", cmd[0]);
    }
    if (!err.empty()) {
      std::cerr << fmt::format("{} {}: {}\n", ymdhms(), funcname(), err);
      std::string dq_err = fmt::format(R"("{}")", err);
      ws->send(server_to_client(13, 1, dq_err));
    }
  }
}

std::string Server::server_to_client(
  unsigned op,
  unsigned error_code,
  const std::string &result) const {
  std::string ret = fmt::format(
R"J({}
  "cmd": {},
  "rc": {},
  "result": {}
{}
)J",
    "{", op, error_code, result, "}");
  std::cerr << "server_to_client: ret=" << ret << '\n';
  return ret;
}

std::string Server::tables_to_json() const {
  std::string ret{"{\n"};
  ret += std::string{"  }\n"};
  return ret;
}

std::string Server::new_table(
  const std::vector<std::string> &cmd,
  WebSocketSession *ws) {
  std::string err;
  const size_t sz = cmd.size();
  std::string table_name, table_password, player_password;
  if ((sz < 3) || (5 < sz)) {
    err = fmt::format("{}: Bad command size: {}", cmd[0], sz);
  } else {
    unsigned pw_flags{};
    auto [_, ec] = std::from_chars(cmd[2].data(), cmd[2].data() + cmd[2].size(),
      pw_flags);
    if ((ec != std::errc()) || (pw_flags > 3)) {
      err = fmt::format("Bad pw_flags: {}", cmd[2]);
    } else {
      size_t n_pw = (pw_flags == 0 ? 0 : (pw_flags == 3 ? 2 : 1));
      if (sz != 3 + n_pw) {
        err = fmt::format("{}: Bad command size: {}, #(pw)={}",
          cmd[0], sz, n_pw);
      } else {
        table_name = cmd[1];
        if (name_table_.find(table_name) != name_table_.end()) {
          err = fmt::format("Table name: {} already in use", table_name);
        } else if (name_table_.size() >= max_tables_) {
          err = fmt::format("Club is full, #(tables)={}", name_table_.size());
        }
        size_t ipw = 2;
        if (pw_flags & 1) {
          table_password = cmd[++ipw];
        }
        if (pw_flags & 2) {
          player_password = cmd[++ipw];
        }
      }
    }
  }
  if (err.empty()) {
    std::unique_ptr<Table> table =
      make_unique<Table>(table_name, player_password, table_password);
    Player *player = table->GetPlayers()[0].get();
    // name_table_[table_name] = table.get();
    name_table_.insert({table_name, std::move(table)});
    // need to delete old table ? !!!!!!!!!!!!!!!!
    ws_player_.insert({ws, player});
  }
  return err;
}

std::string Server::GetTableStatusJson(const Table *table) const {
  std::string json("{\n");
  json += fmt::format(R"j(  "tstate": "{}",\n)j", table->GetTState());
  json += fmt::format(R"j(  "gstate": "{}",\n)j", table->GetGState());
  json += fmt::format(R"j(  "gactive": {},\n)j", int(table->GetGameActive()));
  json += fmt::format(R"j(  "deck": {},\n)j", table->GetDeckSize());
  json += fmt::format(R"j(  "active": [)j");
  const char *sep = "";
  for (uint8_t ci: table->GetCardsActive()) {
    json += fmt::format("{}{}", sep, ci);
  }
  json += "  ]\n";
  json += "}";
  return json;
}
