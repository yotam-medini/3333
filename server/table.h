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
  Table(std::shared_ptr<Player> owner, const std::string& password);
  const std::string &GetName() const;
  const std::vector<uint8_t> &NewGame();
  const std::vector<uint8_t> &GetCardsActive() const { return cards_active_; }
 private:
  void DealCards(size_t n);
  std::vector<std::shared_ptr<Player>> players_; // first is owner
  const std::string password_;
  bool game_active_;
  std::vector<uint8_t> cards_deck_;
  std::vector<uint8_t> cards_active_;
};

#endif /* TABLE_H */
