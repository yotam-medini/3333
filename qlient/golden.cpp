#include "golden.h"
#include <algorithm>
#include <fmt/core.h>

static bool ratio_less_than_golden(long numer, long denom) {
  // Without floating point !
  // Consider ratio numer / denom
  // numer / denom < (1+sqrt(5))/2
  // <=> (2*numer - denom / denom)^2 < 5
  // <=> (2*numer - denom)^2 < 5*denom^2
  long left = 2*numer - denom;
  bool lt = (left*left < 5*denom*denom);
  return lt;
}
  
void Golden::Set(unsigned width, unsigned height, unsigned num_cards) {
  if ((window_size_[0] != width) ||
      (window_size_[1] != height) ||
      (num_cards_ != num_cards)) {
    error_.clear();
    window_size_ = {width, height};
    num_cards_ = num_cards;
    recalculate();
  }
}

Golden::au2_t Golden::GetCardPosition(unsigned card_idx) const {
  au2_t pos{0, 0};
  if (card_idx < num_cards_) {
    unsigned col = card_idx % num_columns_;
    unsigned row = card_idx / num_columns_;
    pos = {
      col * (card_size_[0] + gap_[0]) + gap_[0],
      row * (card_size_[1] + gap_[1]) + gap_[1]};
  }
  return pos;
}

void Golden::recalculate() {
  unsigned best_cols = ComputeBestColumns();
  if (best_cols == unsigned(-1)) {
    error_ = "Failed to compute number of columns";
  } else {
    num_columns_ = best_cols;
    const unsigned num_rows = GetNumRows();
    unsigned xgap = MinGap(window_size_[0]);
    unsigned ygap = MinGap(window_size_[1]);
    unsigned x_avail = window_size_[0] - (num_columns_ + 1)*xgap;
    unsigned y_avail = window_size_[1] - (num_rows + 1)*ygap;
    unsigned xspace = x_avail / num_columns_;
    unsigned yspace = y_avail / num_rows;
    if ((xspace > 6) && (yspace > 6)) {
      // TODO: binary search
      if (ratio_less_than_golden(xspace, yspace)) {
        for (--yspace ; (yspace > 6) && ratio_less_than_golden(xspace, yspace);
             --yspace) {}
      } else {
        for (--xspace; (xspace > 6) && !ratio_less_than_golden(xspace, yspace);
             --xspace) {}
      }
    }
    if ((xspace < 6) || (yspace < 6)) {
      error_ = fmt::format("Card Size {}x{} to small", xspace, yspace);
    } else {
      card_size_ = au2_t{xspace, yspace};
      unsigned gap_avail;
      gap_avail = window_size_[0] - num_columns_*card_size_[0];
      gap_[0] = gap_avail/(num_columns_ + 1);
      gap_avail = window_size_[1] - num_rows*card_size_[1];
      gap_[1] = gap_avail/(num_rows + 1);
    }
  }
}

unsigned Golden::Pick(unsigned x, unsigned y) const {
  unsigned card_idx{unsigned(-1)};
  unsigned col = (x - gap_[0]) / (card_size_[0] + gap_[0]);
  unsigned row = (y - gap_[1]) / (card_size_[1] + gap_[1]);
  unsigned candid = row * num_columns_ + col;
  if (candid < num_cards_) {
    au2_t pos = GetCardPosition(candid);
    if ((x - pos[0] < card_size_[0]) && (y - pos[1] < card_size_[1])) {
      card_idx = candid;
    }
  }
  return card_idx;
}

unsigned Golden::GetNumRows() const {
  return (num_cards_ + num_columns_ - 1) / num_columns_;
}

unsigned Golden::ComputeBestColumns() const {
  // Find the closest rectangle ratio to Golden Ratio g = (1+sqrt(5))/2
  // If we have ratio < g, we convert: ratio := 1/ratio
  unsigned best_numerator = 0, best_denominator = 0;
  unsigned best{unsigned(-1)}; // undefined
  for (int ncols = 1; ncols <= int(num_cards_); ++ncols) {
    const unsigned nrows = (num_cards_ + ncols - 1) / ncols;
    int xgap = MinGap(window_size_[0]);
    int ygap = MinGap(window_size_[1]);
    int numer = (int(window_size_[0]) - (ncols * 1)*xgap) / ncols;
    int denom = (int(window_size_[1]) - (nrows * 1)*ygap) / nrows;
    if ((numer > 0) && (denom > 0)) {
      if (ratio_less_than_golden(numer, denom)) {
        std::swap(numer, denom);
      }
      if ((best == unsigned(-1)) ||
        (numer * best_denominator < best_numerator * denom)) {
        best = ncols;
        best_numerator = numer;
        best_denominator = denom;
      }
    }
  }
  return best;
}        

unsigned Golden::MinGap(unsigned space_size) {
  return std::max<int>(4, space_size/0x20);
}

#if defined(GOLDEN_TEST)

#include <iostream>
#include <string>

using au2_t = std::array<unsigned, 2>;

static int check_disjoint(const Golden& golden) {
  int rc = 0;
  const au2_t card_size = golden.GetCardSize();
  const unsigned num_cards = golden.GetNumCards();
  const unsigned num_cols = golden.GetNumColumns();
  for (unsigned ci = 0; (rc == 0) && (ci < num_cards); ++ci) {
    const au2_t pos = golden.GetCardPosition(ci);
    unsigned col = ci % num_cols;
    unsigned row = ci / num_cols;
    if (col > 0) {
      const unsigned pci = ci - 1;
      const au2_t pos_pre = golden.GetCardPosition(pci);
      if (pos_pre[1] != pos[1]) {
        rc = 1;
        std::cerr << fmt::format("Y-pos of cards {} {} differ\n", pci, ci);
      } else if (pos_pre[0] + card_size[0] >= pos[0]) {
        rc = 1;
        std::cerr << fmt::format("cards {} {} intersect\n", pci, ci);
      }
    }
    if (row > 0) {
      const unsigned pci = ci - num_cols;
      const au2_t pos_pre = golden.GetCardPosition(pci);
      if (pos_pre[0] != pos[0]) {
        rc = 1;
        std::cerr << fmt::format("X-pos of cards {} {} differ\n", pci, ci);
      } else if (pos_pre[1] + card_size[1] >= pos[1]) {
        rc = 1;
        std::cerr << fmt::format("cards {} {} intersect\n", pci, ci);
      }
    }
  }
  return rc;
}

static int check_pick(const Golden &golden) {
  int rc = 0;
  const au2_t window_size = golden.GetWindowSize();
  const au2_t card_size = golden.GetCardSize();
  for (unsigned x = 0; (rc == 0) && (x < window_size[0]); ++x) {
    for (unsigned y = 0; (rc == 0) && (y < window_size[1]); ++y) {
      unsigned expected{unsigned(-1)};
      for (unsigned ci = 0; (rc == 0) && (ci < golden.GetNumCards()); ++ci) {
        au2_t pos = golden.GetCardPosition(ci);
        if ((pos[0] <= x) && (x < pos[0] + card_size[0]) && 
          (pos[1] <= y) && (y < pos[1] + card_size[1])) {
          if (expected != unsigned(-1)) {
            std::cerr << fmt::format("At {}x{} multi expected {} & {}\n",
              x, y, expected, ci);
            rc = 1;
          } else {
            expected = ci;
          }
        }
      }
      if (rc == 0) {
        unsigned picked = golden.Pick(x, y);
        if (picked != expected) {
          std::cerr << fmt::format("At {}x{} picked = {} != {} = expected\n",
            x, y, picked, expected);                                   
          rc = 1;
        }
      }                   
    }
  }
  return rc;
}

static int test_golden(unsigned w, unsigned h, unsigned num_cards) {
  Golden golden(w, h, num_cards);
  int rc = check_disjoint(golden);
  rc = rc ? : check_pick(golden);
  if (rc == 0) {
    const au2_t card_size = golden.GetCardSize();
    std::cout << fmt::format("Card size: {}x{}\n", card_size[0], card_size[1]);
    for (unsigned ci = 0; ci < num_cards; ++ci) {
      const au2_t pos = golden.GetCardPosition(ci);
      std::cout << fmt::format("card[{:2d}] @ {}x{}\n", ci, pos[0], pos[1]);
    }
  }
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
      std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]));
  }
  return rc;
}
#endif
