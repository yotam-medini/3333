// -*- c++ -*-
#if !defined(PLAYER_H)
#define PLAYER_H 1

#include <string>

class Table;

class Player {
 public:
  Player(const std::string& name, const std::string &password) :
    name_(name), password_(password) {
  }
 private:
  const std::string name_;
  const std::string password_;
  Table *table_;
};

#endif /* PLAYER_H */
