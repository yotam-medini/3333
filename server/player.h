// -*- c++ -*-
#if !defined(PLAYER_H)
#define PLAYER_H 1

#include <memory>
#include <string>

class Table;
class WebSocketSession;

class Player {
 public:
  Player(const std::string& name, const std::string &password) :
    name_(name), password_(password) {
  }
  const std::string &GetName() const { return name_; }
  const std::string &GetPassword() const { return password_; }
  const Table *GetTable() const { return table_; }
  void SetTable(Table *table) { table_ = table; }
  void SetWS(WebSocketSession *ws) { ws_ = ws; }
  Table *GetTable() { return table_; }
  WebSocketSession *GetWS() { return ws_; }
  std::string json() const;
 private:
  const std::string name_;
  const std::string password_;
  Table *table_{nullptr};
  WebSocketSession *ws_{nullptr};
  std::string say_;
  unsigned sets_found_{0};
  unsigned bad_calls_{0};
  unsigned add3s_good_{0};
  unsigned add3s_bad_{0};
  unsigned no_more_good_{0};
  unsigned no_more_bad_{0};
  unsigned tcreated_{0};  
  unsigned taction_{0};  
};

#endif /* PLAYER_H */
