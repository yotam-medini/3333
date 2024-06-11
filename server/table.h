// -*- c++ -*-
#if !defined(TABLE_H)
#define TABLE_H 1

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Player;

class Table {
 public:
  using a3i_t = std::array<size_t, 3>;
  Table(
    const std::string& player_name,
    const std::string &player_password,
    const std::string& password);
    const std::string &GetName() const;
    const std::vector<std::unique_ptr<Player>> &GetPlayers() const {
    return players_;
  }
  std::vector<std::unique_ptr<Player>> &GetPlayers() { return players_; }
  void SetTimeLastAction(int t) { time_last_action_ = t; }
  void NewGame();
  void StateBump() { ++tstate_; }
  void GameStateBump();
  bool Try3(const a3i_t& a3i);
  std::string Add3();
  bool NoMore();
  bool GetGameActive() const { return game_active_; }
  size_t GetDeckSize() const { return cards_deck_.size(); }
  const std::vector<uint8_t> &GetCardsActive() const { return cards_active_; }
  int GetTState() const { return tstate_; }
  int GetGState() const { return gstate_; }
  std::string json() const;
 private:
  void DealCards(size_t n);
  bool ActiveHasSet() const;
  std::vector<std::unique_ptr<Player>> players_; // first is owner
  const std::string password_;
  bool game_active_{false};
  std::vector<uint8_t> cards_deck_;
  std::vector<uint8_t> cards_active_;
  int time_last_action_;
  int tstate_{0};
  int gstate_{0};
};

#endif /* TABLE_H */
