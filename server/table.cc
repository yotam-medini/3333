#include "table.h"
#include <cstdlib>
#include <memory>
#include <numeric>
#include <fmt/core.h>
#include "player.h"
#include "utils.h"

Table::Table(
  const std::string& player_name,
  const std::string &player_password,
  const std::string& password) :
  password_(password) {
  players_.push_back(make_unique<Player>(player_name, player_password));
  players_.front()->SetTable(this);
}

const std::string &Table::GetName() const {
  return players_[0]->GetName();
}

void Table::NewGame() {
  cards_deck_ = std::vector<uint8_t>(81);
  std::iota(cards_deck_.begin(), cards_deck_.end(), 0);
  DealCards(12);
}

std::string Table::json() const {
  std::string j("{\n");
  j += fmt::format(R"j(  "tstate": "{}",)j" "\n", tstate_);
  j += fmt::format(R"j(  "gstate": "{}",)j" "\n", gstate_);
  j += fmt::format(R"j(  "gactive": {},)j" "\n", int(game_active_));
  j += fmt::format(R"j(  "players": [)j");
  const char *sep = "\n";
  for (auto &player: players_) {
    j += sep;
    j += indent(player->json(), 2);
    sep = ",\n";
  }
  j += "]";
  j += fmt::format(R"j(  "deck": {},)j" "\n", GetDeckSize());
  j += fmt::format(R"j(  "active": [)j");
  sep = "";
  for (uint8_t ci: cards_active_) {
    j += fmt::format("{}{}", sep, ci); sep = ", ";
  }
  j += "]\n";
  j += "}";
  return j;
}
void Table::DealCards(size_t n) {
  for (size_t i = 0; i < n; ++i) {
    size_t ci = rand() % cards_deck_.size();
    cards_active_.push_back(cards_deck_[ci]);
    cards_deck_[ci] = cards_deck_.back();
    cards_deck_.pop_back();
  }
}

