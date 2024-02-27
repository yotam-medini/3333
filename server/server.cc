#include "server.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "net.h"
#include "http_session.h"
#include "utils.h"

class WebSocketSession;

class NetServer {
 public:
  NetServer(const std::string &host, uint16_t port) :
    host_{host},
    port_(port),
    acceptor_{ioc_},
    socket_{ioc_} {
  }
  void run() {
    auto address = net::ip::make_address(host_);
    tcp::endpoint endpoint{address, port_};
    error_code ec;
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
      std::cerr << "acceptor.open failed, ec=" << ec << '\n';
    } else {
      acceptor_.set_option(net::socket_base::reuse_address(true));
      if (ec)
      {
        std::cerr << "acceptor.set_option failed, ec=" << ec << '\n';
      }
    }
    if (!ec) {
      acceptor_.bind(endpoint, ec);
      if (ec)
      {
        std::cerr << "acceptor.bind failed, ec=" << ec << '\n';
      }
    }
    if (!ec) {
      acceptor_.listen(net::socket_base::max_listen_connections, ec);
      if (ec)
      {
        std::cerr << "acceptor.listen failed, ec=" << ec << '\n';
      }
    }
    if (!ec) {
      // Start accepting a connection
      accept_next();
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
      HttpSession *hs = new HttpSession(
        std::move(socket_),
        // register_ws
        [this](HttpSession *dhs, WebSocketSession *ws) {
          hs_to_ws_[dhs] = ws;
        },
        // unregister http
        [this](HttpSession *dhp) {
          WebSocketSession *ws = hs_to_ws_[dhp];
          if (ws) { // should move to upper server
            auto iter = ws_player_.find(ws);
            Player *player = iter->second;
            if (player) {
              std::cerr << funcname() << " need to delete player\n";
            }
            ws_player_.erase(iter);
          }
          this->hs_to_ws_.erase(dhp);
        });
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
  std::unordered_map<HttpSession*, WebSocketSession*> hs_to_ws_;
  std::unordered_map<WebSocketSession*, Player*> ws_player_;
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
    net_server_{new NetServer{host, port}} {
  std::ofstream(pidfn_) << getpid() << '\n';
  net_server_->run();
}

Server::~Server() {
  delete net_server_;
}

void Server::run() {
}
