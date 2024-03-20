// -*- c++ -*-
#if !defined(PLAYER_H)
#define PLAYER_H 1

#include <memory>
#include <string>

class Table;

class Player {
 public:
  Player(const std::string& name, const std::string &password) :
    name_(name), password_(password) {
  }
  const std::string &GetName() const { return name_; }
  const std::string &GetPassword() const { return password_; }
  const Table *GetTable() const { return table_; }
  Table *GetTable() { return table_; }
 private:
  const std::string name_;
  const std::string password_;
  Table *table_;
};

#endif /* PLAYER_H */
