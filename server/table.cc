#include "table.h"
#include "player.h"

Table::Table(Player* owner, const std::string& password) :
  password_(password) {
  players_.push_back(owner);
}

const std::string &Table::get_name() const {
  return players_[0]->get_name();
}
