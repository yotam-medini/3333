// -*- c++ -*-
#if !defined(CARD_H)
#define CARD_H 1

#include <cstdint>
#include <array>

using card_t = std::array<uint8_t, 4>;

extern const std::array<card_t, 81> all_cards;

extern bool IsSet(
  const card_t &card0,
  const card_t &card1,
  const card_t &card2);

#endif /* CARD_H */
