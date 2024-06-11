#include "table.h"
#include <cstdlib>
#include <array>
#include <memory>
#include <numeric>
#include <fmt/core.h>
#include "player.h"
#include "card.h"
#include "utils.h"

Table::Table(
  const std::string& player_name,
  const std::string &player_password,
  const std::string& password) :
  password_(password),
  time_last_action_(GetTime()) {
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
  game_active_ = true;
  GameStateBump();
}

void Table::GameStateBump() {
  time_last_action_ = GetTime();
  StateBump();
  ++gstate_;
}

std::string Table::json() const {
  std::string j("{\n");
  j += fmt::format(R"j(  "tstate": {},)j" "\n", tstate_);
  j += fmt::format(R"j(  "gstate": {},)j" "\n", gstate_);
  j += fmt::format(R"j(  "gactive": {},)j" "\n", int(game_active_));
  j += fmt::format("  {}: ", dq("players"));
  j += "{\n";                   
  const char *sep = "";
  for (auto &player: players_) {
    j += sep;
    j += fmt::format("    {}:", dq(player->GetName()));
    j += Indent(player->json(), 4);
    sep = ",\n";
  }
  j += "\n  },\n";
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

bool Table::Try3(const a3i_t& a3i) {
  bool found = false;
  const card_t &card0 = all_cards[cards_active_[a3i[0]]];
  const card_t &card1 = all_cards[cards_active_[a3i[1]]];
  const card_t &card2 = all_cards[cards_active_[a3i[2]]];
  if (IsSet(card0, card1, card2)) {
    a3i_t dec_a3i{a3i};
    sort(dec_a3i.begin(), dec_a3i.end(), std::greater<>());
    for (size_t i = 0; i < 3; ++i) {
      cards_active_[dec_a3i[i]] = cards_active_.back();
      cards_active_.pop_back();
    }
    if ((cards_active_.size() < 12) && (GetDeckSize() >= 3)) {
      DealCards(3);
    }
    GameStateBump();
    found = true;
  } else {
    StateBump();
  }
  return found;
}

std::string Table::Add3() {
  std::string err;
  if (ActiveHasSet()) {
    err = "Adding cards was not necessary";
  }
  if (cards_deck_.size() < 3) {
    err = "Deck size < 3";
  } else {
    DealCards(3);
    GameStateBump();
  }
  return err;
}

bool Table::NoMore() {
  bool good_call = !ActiveHasSet();
  if (good_call) {
    game_active_ = false;
    GameStateBump();
  } else {
    StateBump();
  }
  return good_call;
}
bool Table::ActiveHasSet() const {
  bool has = false;
  const size_t na = cards_active_.size();
#if 0
  for (size_t i = 0; (!has) && (i < na); ++i) {
    const card_t card0 = all_cards[i];
    for (size_t j = i + 1; (!has) && (j < na); ++j) {
      const card_t card1 = all_cards[j];
      for (size_t k = j + 1; (!has) && (k < na); ++k) {
        const card_t card2 = all_cards[k];
        has = IsSet(card0, card1, card2);
      }
    }
  }
#else
  std::array<bool, 81> cards_exist;
  cards_exist.fill(false);
  for (size_t i = 0; i < na; ++i) {
    cards_exist[cards_active_[i]] = true;
  }
  for (size_t i = 0; i < na; ++i) {
    size_t ci = cards_active_[i];
    for (size_t j = i + 1; j < na; ++j) {
      size_t cj = cards_active_[j];
      const size_t ck = GetIndexToCompleteSet(ci, cj);
      if (cards_exist[ck]) {
        has = true;
        i = j = na;
      }
    }
  }
#endif
  return has;
}
