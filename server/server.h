// -*- c++ -*-
#if !defined(SERVER_H)
#define SERVER_H 1

#include <string>
#include <unordered_map>
#include <unordered_set>

class Table;
class Player;
class WebSocketSession;

class Server {
 public:
  void run();
 private:
  std::unordered_map<std::string, Table*> name_table_;
  std::unordered_map<WebSocketSession*, Player*> ws_player_;
};

#endif /* SERVER_H */
