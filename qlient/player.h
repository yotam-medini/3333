#pragma once
#include <string>

class Player {
 public:
  Player();
  void reset();
  std::string name_;
  int score() const;
  unsigned set_calls_good_{0};
  unsigned set_calls_bad_{0};
  unsigned add3_good_{0};
  unsigned add3_bad_{0};
  unsigned no_more_good_{0};
  unsigned no_more_bad_{0};
  unsigned tcreated_{0};
  unsigned taction_{0};
};
