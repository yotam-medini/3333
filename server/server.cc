#include "server.h"
#include <fstream>
#include <unistd.h>

Server::Server(
  const std::string &host,
  const size_t max_tables,
  const size_t max_players,
  const unsigned expire_seconds,
  const std::string pidfn,
  const unsigned debug_flags
) :
    host_{host},
    max_tables_{max_tables},
    max_players_{max_players},
    expire_seconds_{expire_seconds},
    pidfn_{pidfn},
    debug_flags_{debug_flags} {
}

Server::~Server() {
  std::ofstream(pidfn_) << getpid() << '\n';
}

void Server::run() {
}
