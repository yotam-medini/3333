#include "card.h"
#include <fmt/format.h>

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

size_t GetAllCardsIndex(const card_t &card) {
  size_t index = 0;
  for (size_t cat = 0, trip = 1; cat < 4; ++cat, trip *= 3) {
    index += trip * card[cat];
  }
  return index;
}

std::size_t GetIndexToCompleteSet(
  std::size_t card_index0,
  std::size_t card_index1) {
  const card_t &card0 = all_cards[card_index0];
  const card_t &card1 = all_cards[card_index1];
  card_t card2;
  for (size_t cat = 0; cat < 4; ++cat) {
    const size_t a = card0[cat] + card1[cat];
    card2[cat] = (6 - a) % 3;
  }
  size_t index = GetAllCardsIndex(card2);
  return index;
}


bool IsSet(
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

#if TEST_CARD == 1
#include <iostream>
int main(int argc, char **argv) {
  int rc = 0;
  for (size_t i = 0; (rc == 0) && (i < 81); ++i) {
    const size_t ci = GetAllCardsIndex(all_cards[i]);
    if (ci != i) {
      rc = 1;
      std::cerr << fmt::format("GetAllCardsIndex(all_cards[{}]) = {}\n", i, ci);
    }
  }
  for (size_t i = 0; (rc == 0) && (i < 81); ++i) {
    const card_t &card0 = all_cards[i];
    for (size_t j = 0; (rc == 0) && (j < 81); ++j) {
      const card_t &card1 = all_cards[j];
      for (size_t k = (i != j ? 0 : 81); (rc == 0) && (k < 81); ++k) {
        const card_t &card2 = all_cards[k];
        if (IsSet(card0, card1, card2) != (k == GetIndexToCompleteSet(i, j))) {
          rc = 1;
          std::cerr << fmt::format("Inconsistency with IsSet({}, {}, {})\n",
            i, j, k);
        }
      }
    }
  }
  return rc;
}
#endif
