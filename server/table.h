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
  static void set_initial_cards_deck(const std::string &comma_separated);
  Table(
    const std::string &player_name,
    const std::string &player_password,
    const std::string &password);
  ~Table();
  void Close();
  const std::string &GetName() const { return name_; }
  const std::string &GetPassword() const { return password_; }
  const std::vector<std::unique_ptr<Player>> &GetPlayers() const {
    return players_;
  }
  std::vector<std::unique_ptr<Player>> &GetPlayers() { return players_; }
  Player *AddPlayer(
    const std::string& player_name,
    const std::string& player_password);
  void DeletePlayer(Player *player);
  void SetTimeLastAction(int t) { time_last_action_ = t; }
  void NewGame();
  void StateBump();
  void GameStateBump();
  bool Try3(const a3i_t& a3i);
  void BumpSetsFound() { ++sets_found_; }
  std::string Add3();
  bool NoMore();
  bool GetGameActive() const { return game_active_; }
  size_t GetDeckSize() const { return cards_deck_.size(); }
  const std::vector<uint8_t> &GetCardsActive() const { return cards_active_; }
  int GetTState() const { return tstate_; }
  int GetGState() const { return gstate_; }
  std::string json() const;
  std::string GetJsonSummary() const;
  unsigned GetTimeCreated() const;
 private:
  void DealCards(size_t n);
  bool ActiveHasSet() const;
  static std::vector<uint8_t> initial_cards_deck_;
  const std::string name_; // == first player name
  const std::string password_;
  std::vector<std::unique_ptr<Player>> players_; // first is owner
  bool game_active_{false};
  int sets_found_{0};
  std::vector<uint8_t> cards_deck_;
  std::vector<uint8_t> cards_active_;
  int time_last_action_;
  int tstate_{0};
  int gstate_{0};
};

#endif /* TABLE_H */
