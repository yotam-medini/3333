#include "server.h"
#include <cstdint>
#include <fstream>
#include <unistd.h>

#include "net.h"

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
    tcp::acceptor acceptor(ioc_);
  }
  const std::string host_;
  const uint16_t port_;
  net::io_context ioc_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
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
