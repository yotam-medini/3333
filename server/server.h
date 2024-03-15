// -*- c++ -*-
#if !defined(SERVER_H)
#define SERVER_H 1

#include <cstdint>
#include <string>
#include <unordered_map>

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
  void ws_deleted(WebSocketSession *ws);
  void ws_send_message(WebSocketSession *ws, const std::string& message);
  void ws_received_message(WebSocketSession *ws, const std::string& message);
  std::string server_to_client(
    unsigned op,
    unsigned error_code,
    const std::string &result) const;
  std::string tables_to_json() const;
  const size_t max_tables_;
  const size_t max_players_;
  const unsigned expire_seconds_;
  const std::string pidfn_;
  const unsigned debug_flags_;
  NetServer *net_server_;
  std::unordered_map<WebSocketSession*, Player*> ws_player_;
  std::unordered_map<std::string, Table*> name_table_;
};

#endif /* SERVER_H */
