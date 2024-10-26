#pragma once
#include <array>
#include <string>

class Golden {
 public:
  using au2_t = std::array<unsigned, 2>;
  Golden(unsigned width=0, unsigned height=0, unsigned num_cards=0) :
    window_size_{width, height}, num_cards_{num_cards} {
    recalculate();
  }
  void set(unsigned width, unsigned height, unsigned num_cards);
  unsigned GetNumCards() const { return num_cards_; }
  au2_t GetCardPosition(unsigned card_idx) const;
  const au2_t &GetWindowSize() const { return window_size_; }
  const au2_t &GetCardSize() const { return card_size_; }
  unsigned pick(unsigned x, unsigned y) const;
  unsigned pick(const au2_t &pt) const { return pick(pt[0], pt[1]); }
  unsigned GetNumColumns() const { return num_columns_; }
  unsigned GetNumRows() const;
  const std::string &GetError() const { return error_; }
 private:
  static unsigned MinGap(unsigned space_size);
  unsigned ComputeBestColumns() const;
  std::string error_;
  void recalculate();
  au2_t window_size_;
  unsigned num_cards_;
  au2_t card_size_;
  unsigned num_columns_{1};
  au2_t gap_;
};
