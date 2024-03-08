// -*- c++ -*-
#if !defined(TABLE_H)
#define TABLE_H 1

#include <string>
#include <vector>

class Player;

class Table {
 public:
  Table(Player* owner, const std::string& password);
 private:
  std::vector<Player*> players_; // first is owner
  const std::string password_;
  bool game_active_;
};

#endif /* TABLE_H */
