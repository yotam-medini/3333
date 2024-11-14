#include "server.h"
#include <cstdint>
#include <iostream>
#include <functional>
#include <fstream>
#include <memory>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <boost/asio/ssl/context.hpp>
#include <fmt/core.h>

#include "net.h"
#include "http_session.h"
#include "utils.h"

#include "logger.h"
#include "player.h"
#include "table.h"
#include "ws_session.h"
#include "cs_consts.h"
#include "net_utils.h"
#include "ssl_ws.h"

class WebSocketSession;
using notify_ws_t = std::function<void(WebSocketSession*)>;
using report_message_t =
  std::function<void(WebSocketSession*, const std::string&)>;
using ssl_ctx = net::ssl::context;

class NetServer {
 public:
  NetServer(
    Logger *pLogger,
    const std::string &host,
    uint16_t port,
    ssl_ctx *ssl_context,
    notify_ws_t notify_ws_add,
    notify_ws_t notify_ws_delete,
    report_message_t report_message) :
    pLogger_{pLogger},
    host_{host},
    port_(port),
    // ssl_context_{ssl_context},
    acceptor_{ioc_},
    socket_{ioc_},
    // ssl_ws_{ioc_},
    notify_ws_add_{notify_ws_add},
    notify_ws_delete_{notify_ws_delete},
    report_message_{report_message} {
  }
  void run() {
    pLogger_->log("NetServer::run");
    const std::string fn = FuncName();
    error_code ec;
    tcp::endpoint endpoint;
    auto address = net::ip::make_address(host_, ec);
    if (ec) {
      std::cerr << fn << " make_address failed, ec=" << ec << '\n';
    } else {
      endpoint = tcp::endpoint{address, port_};
      const std::string  message  = fmt::format("endpoint={}:{}",
        endpoint.address().to_string(), endpoint.port());
      std::cerr << message << '\n';
      pLogger_->log(message);
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
    const std::string msg = fmt::format("{} {}: accept_next",
      FuncName(), YMDHMS());
    std::cerr << msg << '\n';
    pLogger_->log(msg);
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
      pLogger_->log(fmt::format("accept connection from {}",
        SocketGetPeerAddress(socket_.native_handle())));
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
  Logger *pLogger_;
  const std::string host_;
  const uint16_t port_;
  ssl_ctx *ssl_context_;
  net::io_context ioc_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  // ssl_ws ssl_ws_;
  notify_ws_t notify_ws_add_;
  notify_ws_t notify_ws_delete_;
  report_message_t report_message_;
  std::unordered_map<HttpSession*, WebSocketSession*> hs_to_ws_;
};

Server::Server(
  const std::string &host,
  uint16_t port,
  const std::string &sslcert,
  const std::string &sslprivate,
  const size_t max_tables,
  const size_t max_players,
  const unsigned expire_seconds,
  const std::string &pidfn,
  const unsigned debug_flags
) :
    pLogger_{std::make_unique<Logger>(
      "3333-server.log", "3333-server-backup.log")},
    max_tables_{max_tables},
    max_players_{max_players},
    expire_seconds_{expire_seconds},
    pidfn_{pidfn},
    debug_flags_{debug_flags},
    net_server_{nullptr} {
  std::unique_ptr<ssl_ctx> ssl_context;
  if (!sslcert.empty()) {
    ssl_context = std::make_unique<ssl_ctx>(ssl_ctx::tlsv12_server);
    ssl_context->set_options(
      ssl_ctx::default_workarounds |
      ssl_ctx::no_sslv2 |
      ssl_ctx::single_dh_use);
      ssl_context->use_certificate_chain_file(sslcert);
      ssl_context->use_private_key_file(sslprivate, ssl_ctx::pem);
  }
  net_server_ = new NetServer{
    pLogger_.get(),
    host,
    port,
    ssl_context.get(),
    [this](WebSocketSession *ws) -> void {
      this->ws_player_[ws] = nullptr;
    },
    [this](WebSocketSession *ws) -> void { this->WsDeleted(ws); },
    [this](WebSocketSession *ws, const std::string& message) -> void {
      this->WsReceivedMessage(ws, message);
    }
  };
}
    
Server::~Server() {
  delete net_server_;
}

void Server::run() {
  std::ofstream(pidfn_) << getpid() << '\n';
  pLogger_->log(fmt::format("Server::run pid={}", getpid()));
  net_server_->run();
}

void Server::WsDeleted(WebSocketSession *ws) {
  if (debug_flags_ & 0x1) {
    std::cerr << __func__ << '\n';
  }
  auto iter = ws_player_.find(ws);
  if (iter != ws_player_.end()) {
    Player *player = iter->second;
    if (player) {
      Table *table = player->GetTable();
      if (table) {
        table->DeletePlayer(player);
        UpdateTableGstate(table);
      }
    }
    ws_player_.erase(iter);
  }
}

Player *Server::WsGetPlayer(WebSocketSession *ws) {
  auto iter = ws_player_.find(ws);
  Player *player = (iter != ws_player_.end() ? iter->second : nullptr);
  return player;
}

void Server::WsReceivedMessage(
  WebSocketSession *ws,
  const std::string &message) {
  std::vector<std::string> cmd = SSplit(message);
  if (debug_flags_ & 0x1) {
    std::cerr << YMDHMS() << ' ' << FuncName() <<
      " message=" << message << '\n';
    std::cerr << fmt::format("{} {} message='{}', #(cmd)={}\n",
      YMDHMS(), FuncName(), message, cmd.size());
  }
  if (!cmd.empty()) {
    std::string err;
    Player *player = WsGetPlayer(ws);
    Table *table = nullptr;
    if (player) {
      table = player->GetTable();
      player->SetTAction();
      table->SetTimeLastAction(player->GetTAction());
    }
    if (!CheckCommandGStateOK(cmd, table)) {
      std::cerr << "Unsynced Game State for command: " << cmd[0] << '\n';
    } else if (cmd[0] == S3333_C2S_TBLS) {
      ws->send(ServerToClient(E3333_S2C_TBLS, 0, TablesToJson()));
    } else if (cmd[0] == S3333_C2S_NTBL) {
      if (player) {
        DeletePlayer(ws, player);
      }
      std::string table_name;
      err = NewTable(cmd, table_name, ws);
      if (err.empty()) {
        auto result = brace(fmt::format(R"j( "table_name": {})j",
          dq(table_name)));
        ws->send(ServerToClient(E3333_S2C_NTBL, 0, result));
      }
    } else if (cmd[0] == S3333_C2S_JOIN) {
      if (player) {
        DeletePlayer(ws, player);
      }
      std::string table_name;
      std::string player_name;
      err = Join(cmd, table, table_name, player_name, ws);
      if (err.empty()) {
        auto result = brace(
          fmt::format(R"j( "table_name": {}, "player_name": {})j",
          dq(table_name), dq(player_name)));
        ws->send(ServerToClient(E3333_S2C_JOIN, 0, result));
        UpdateTableGstate(table);
      }
    } else if (cmd[0] == S3333_C2S_GNEW) {
      table->NewGame();
      UpdateTableGstate(table);
    } else if (cmd[0] == S3333_C2S_ADD3) {
      // need to check time
      err = table->Add3();
      if (err.empty()) {
        player->BumpAdd3Good();
      } else {
        player->BumpAdd3Bad();
      }
      UpdateTableGstate(table); // even if error
    } else if (cmd[0] == S3333_C2S_TRY3) {
      err = Try3(player, table, cmd);
      UpdateTableGstate(table); // even if error
    } else if (cmd[0] == S3333_C2S_NMOR) {
      if (table->NoMore()) {
         player->BumpNoMoreGood();
      } else {
         player->BumpNoMoreBad();
      }
      UpdateTableGstate(table);
    } else if (cmd[0] == S3333_C2S_PLRF) {
      if (table) {
        ws->send(ServerToClient(E3333_S2C_PLAYERS, 0, PlayersToJson(table)));
      }
    } else {
      err = fmt::format("Unsupported command='{}'", cmd[0]);
    }
    if (!err.empty()) {
      std::cerr << fmt::format("{} {}: {}\n", YMDHMS(), FuncName(), err);
      std::string dq_err = fmt::format(R"("{}")", err);
      ws->send(ServerToClient(13, 1, dq_err));
    }
  }
}

std::string Server::ServerToClient(
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
  std::cerr << "ServerToClient: ret=" << ret << '\n';
  return ret;
}

std::string Server::TablesToJson() const {
  std::string ret{"{\n"};
  const char *eot = "";
  for (auto const& [name, table]: name_table_) {
    ret += eot;
    ret += fmt::format("    {}: {}",
      dq(name), Indent(table->GetJsonSummary(), 4));
    eot = ",\n";
  }
  ret += std::string{"\n  }"};
  return ret;
}

std::string Server::PlayersToJson(const Table *table) const {
  std::string ret{"[\n"};
  const char *eot = "";
  auto &players = table->GetPlayers();
  for (size_t pi = 0; pi < players.size(); ++pi) {
    ret += eot;
    ret += Indent(players[pi]->json(), 4, true);
    eot = ",\n";
  }
  ret += std::string{"\n  ]"};
  return ret;
}

void Server::DeletePlayer(WebSocketSession *ws, Player *player) {
  Table *table = player->GetTable();
  auto &players = table->GetPlayers();
  const bool owner = players[0].get() == player;
  if (owner) {
    table->Close();
    for (size_t pi = 1; pi < players.size(); ++pi) {
      players[pi]->GetWS()->send(
        ServerToClient(E3333_S2C_TABLE_CLOSED, 0, "{}"));
    }
    name_table_.erase(table->GetName());
  } else {
    table->DeletePlayer(player);
    UpdateTableGstate(table);
  }
  ws_player_.erase(ws);
}

bool Server::CheckCommandGStateOK(const cmd_t &cmd, const Table *table) const {
  bool ok = true;
  if ((cmd[0] == S3333_C2S_ADD3) ||
      (cmd[0] == S3333_C2S_NMOR) ||
      (cmd[0] == S3333_C2S_TRY3)) {
    const int gstate = (cmd.size() > 1 ? StrToInt(cmd[1], -1) : -1);
    ok = table && (gstate == table->GetGState());
  }
  return ok;
}


std::string Server::NewTable(
  const cmd_t &cmd,
  std::string &table_name,
  WebSocketSession *ws) {
  std::string err;
  const size_t sz = cmd.size();
  std::string table_password, player_password;
  if ((sz < 3) || (5 < sz)) {
    err = fmt::format("{}: Bad command size: {}", cmd[0], sz);
  } else {
    table_name = cmd[1];
    if (name_table_.find(table_name) != name_table_.end()) {
      err = fmt::format("Table name: {} already in use", table_name);
    } else if (name_table_.size() >= max_tables_) {
      err = fmt::format("Club is full, #(tables)={}", name_table_.size());
    }
  }
  if (err.empty()) {
    err = GetCommandPasswords(cmd, 2, table_password, player_password);
  }
  if (err.empty()) {
    std::unique_ptr<Table> table =
      make_unique<Table>(table_name, player_password, table_password);
    Player *player = table->GetPlayers()[0].get();
    player->SetWS(ws);
    // name_table_[table_name] = table.get();
    name_table_.insert({table_name, std::move(table)});
    // need to delete old table ? !!!!!!!!!!!!!!!!
    ws_player_.insert({ws, player});
  }
  return err;
}

std::string Server::Join(
  const cmd_t &cmd,
  Table *&table,
  std::string &table_name,
  std::string &player_name,
  WebSocketSession *ws) {
  std::string err;
  const size_t sz = cmd.size();
  std::string table_password, player_password;
  table = nullptr;
  if ((sz < 4) || (6 < sz)) {
    err = fmt::format("{}: Bad command size: {}", cmd[0], sz);
  } else {
    table_name = cmd[1];
    player_name = cmd[2];
    err = GetCommandPasswords(cmd, 3, table_password, player_password);
    if (err.empty()) {
      auto iter = name_table_.find(table_name);
      if (iter == name_table_.end()) {
        err = fmt::format("Table name: {} not found", table_name);
      } else {
        table = iter->second.get();
        if (table_password != table->GetPassword()) {
          err = fmt::format("Table Password not matched");
        } else {
          // Need to check for duplicate player_name or rejoining
          size_t np = iter->second->GetPlayers().size();
          if (np >= max_players_) {
            err = fmt::format("table {} is full, #(players)={}",
              table_name, np);
          }
        }
      }
    }
  }
  if (err.empty()) {
    Player *player = table->AddPlayer(player_name, player_password);
    player->SetWS(ws);
    ws_player_.insert({ws, player});
  }
  return err;
}

std::string Server::GetCommandPasswords(
  const cmd_t &cmd,
  size_t i,
  std::string &table_password,
  std::string &player_password) const {
  std::string err;
  int pw_flags = StrToInt(cmd[i], -1);
  if ((pw_flags < 0) || (pw_flags > 3)) {
    err = fmt::format("Bad pw_flags: {}", cmd[2]);
  } else {
    const size_t sz = cmd.size();
    const size_t n_pw = (pw_flags == 0 ? 0 : (pw_flags == 3 ? 2 : 1));
    if (sz != i + 1 + n_pw) {
      err = fmt::format("{}: Bad command size: {}, #(pw)={}",
        cmd[0], sz, n_pw);
    } else {
      if (pw_flags & 1) {
        table_password = cmd[++i];
      }
      if (pw_flags & 2) {
        player_password = cmd[++i];
      }
    }
  }
  return err;
}

void Server::UpdateTableGstate(Table *table) {
  const std::string jts = Indent(table->json(), 2);
  for (auto &tplayer: table->GetPlayers()) {
    tplayer->GetWS()->send(ServerToClient(E3333_S2C_GSTATE, 0, jts));
  }
}

std::string Server::Try3(Player *player, Table *table, const cmd_t &cmd) {
  std::string err;
  Table::a3i_t a3i;
  if (cmd.size() != 5) {
    err = fmt::format("Bad command {} size: {} != 5", cmd[0], cmd.size());
  } else {
    int gstate = StrToInt(cmd[1], -1);
    if (gstate == -1) {
      err = fmt::format("Bad gstate: {}", cmd[1]);
    } else {
      for (size_t i = 0; err.empty() && (i < 3); ++i) {
        int ai = StrToInt(cmd[2 + i]);
        if (ai == -1) {
          err = fmt::format("Bad active card index: {}", cmd[2 + i]);
        } else {
          a3i[i] = ai;
        }
      }
    }
  }
  if (err.empty()) {
    if (table->Try3(a3i)) {
      table->BumpSetsFound();
      player->BumpSetsFound();
    } else {
      player->BumpBadCalls();
      player->GetWS()->send(ServerToClient(E3333_S2C_NOT_A_SET, 0, "{}"));
    }
  }
  return err;
}
