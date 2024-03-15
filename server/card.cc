#include "card.h"

static std::array<card_t, 81> compute_all_cards() {
  std::array<card_t, 81> cards81;
  for (uint8_t i = 0; i < 81; ++i) {
    card_t &card = cards81[i];
    uint8_t n = i;
    for (uint8_t cat = 0; cat < 4; ++cat) {
      card[cat] = n % 3;
      n /= 3;
    }
  }
  return cards81;
}

const std::array<card_t, 81> all_cards = compute_all_cards();

bool is_set(
  const card_t& card0,
  const card_t& card1,
  const card_t& card2) {
  bool ret = true;
  auto mod3 = [](uint8_t v0, uint8_t v1, uint8_t v2) -> bool {
    static const unsigned bits_mod3 = (1u << 0) | (1u << 3) | (1u << 6);
    const unsigned bit = 1u << (v0 + v1 + v2);
    return (bit & bits_mod3) != 0;
  };
  for (unsigned cat = 0; ret && (cat < 4); ++cat) {
    ret = mod3(card0[cat], card1[cat], card2[cat]);
  }
  return ret;
}
