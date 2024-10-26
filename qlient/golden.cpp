#include "golden.h"

void Golden::set(unsigned width, unsigned height, unsigned num_cards) {
  if ((window_size_[0] != width) ||
      (window_size_[1] != height) ||
      (num_cards_ != num_cards)) {
    window_size_ = {width, height};
    num_cards_ = num_cards;
    recalculate();
  }
}

Golden::au2_t Golden::GetCardPosition(unsigned card_idx) const {
  au2_t pos{0, 0};
  if (card_idx < num_cards_) {
    pos = {
      column_offsets_[card_idx % num_cards_],
      row_offsets_[card_idx / num_cards_]};
  }
  return pos;
}

void Golden::recalculate() {
}

unsigned Golden::pick(unsigned x, unsigned y) const {
  unsigned card_idx{unsigned(-1)};
  return card_idx;
}

unsigned Golden::GetNumRows() const {
  return (num_cards_ + n_columns_ - 1) / n_columns_;
}

#if defined(GOLDEN_TEST)

#include <iostream>
#include <string>
#include <fmt/core.h>

using au2_t = std::array<unsigned, 2>;

static int check_disjoint(const Golden& golden) {
  int rc;
  const au2_t card_size = golden.GetCardSize();
  const unsigned num_cards = golden.GetNumCards();
  const unsigned num_cols = golden.GetNumColumns();
  for (unsigned ci = 0; (rc == 0) && (ci < num_cards); ++ci) {
    const au2_t pos = golden.GetCardPosition(ci);
    unsigned col = ci % num_cols;
    unsigned row = ci / num_cols;
    if (col > 0) {
      const au2_t pos_pre = golden.GetCardPosition(ci - 1);
      if (pos_pre[1] != pos[1]) {
        rc = 1;
        std::cerr << fmt::format("Heights of cards {} {} differ\n", ci-1, ci);
      } else if (pos_pre[0] + card_size[0] >= pos[0]) {
        std::cerr << fmt::format("cards {} {} intersect\n", ci-1, ci);
      }
    }
  }
  return rc;
}

static int test_golden(unsigned w, unsigned h, unsigned num_cards) {
  int rc = 0;
  Golden golden(w, h, num_cards);
  rc = check_disjoint(golden);
  return rc;
}

static void usage(const char *p0) {
  std::cerr << fmt::format("Usage: {} <width> <height> <num_cards>\n", p0);
}

int main(int argc, char **argv) {
  int rc = 0;
  if (argc != 1+3) {
    usage(argv[0]);
    rc = 1;
  } else {
    rc = test_golden(
      std::stoi(argv[0]), std::stoi(argv[1]), std::stoi(argv[2]));
  }
  return rc;
}
#endif
