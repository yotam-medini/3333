// -*- c++ -*-
#if !defined(SERVER_H)
#define SERVER_H 1

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Player;
class Table;
class WebSocketSession;
class NetServer;

class Server {
 public:
  Server(
    const std::string &host,
    const uint16_t port,
    const size_t max_tables,
    const size_t max_players,
    const unsigned expire_seconds,
    const std::string &pidfn,
    const unsigned debug_flags
  );
  ~Server();
  void run();
 private:
  void WsDeleted(WebSocketSession *ws);
  void WsSendMessage(WebSocketSession *ws, const std::string& message);
  void WsReceivedMessage(WebSocketSession *ws, const std::string& message);
  std::string ServerToClient(
    unsigned op,
    unsigned error_code,
    const std::string &result) const;
  std::string TablesToJson() const;
  std::string NewTable(
    const std::vector<std::string> &cmd,
    WebSocketSession *ws);
  const size_t max_tables_;
  const size_t max_players_;
  const unsigned expire_seconds_;
  const std::string pidfn_;
  const unsigned debug_flags_;
  NetServer *net_server_;
  std::unordered_map<WebSocketSession*, Player*> ws_player_;
  std::unordered_map<std::string, std::unique_ptr<Table>> name_table_;
};

#endif /* SERVER_H */
