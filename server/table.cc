#include "table.h"
#include <cstdlib>
#include <memory>
#include <numeric>
#include "player.h"

Table::Table(std::shared_ptr<Player> owner, const std::string& password) :
  password_(password) {
  players_.push_back(owner);
}

const std::string &Table::GetName() const {
  return players_[0]->GetName();
}

const std::vector<uint8_t> &Table::NewGame() {
  cards_deck_ = std::vector<uint8_t>(81);
  std::iota(cards_deck_.begin(), cards_deck_.end(), 0);
  DealCards(12);
  return GetCardsActive();
}

void Table::DealCards(size_t n) {
  for (size_t i = 0; i < n; ++i) {
    size_t ci = rand() % cards_deck_.size();
    cards_active_.push_back(cards_deck_[ci]);
    cards_deck_[ci] = cards_deck_.back();
    cards_deck_.pop_back();
  }
}

