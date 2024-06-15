#include "net_utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fmt/core.h>

std::string SocketGetPeerAddress(int sockfd) {
  std::string ret;
  struct sockaddr_in peer_addr;
  socklen_t addr_len = sizeof(peer_addr);

  if (getpeername(sockfd, (struct sockaddr*)&peer_addr, &addr_len) == 0) {
    char ip_addr[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(peer_addr.sin_addr), ip_addr, INET_ADDRSTRLEN)) {
      ret = fmt::format("{}:{}", ip_addr, peer_addr.sin_port);
    }
  }
  return ret;
}
