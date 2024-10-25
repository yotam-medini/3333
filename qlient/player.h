#pragma once
#include <string>

class Player {
 public:
  Player();
  void reset();
  std::string name_;
  unsigned set_calls_good{0};
  unsigned set_calls_bad{0};
  unsigned add3_good_{0};
  unsigned add3_bad{0};
  unsigned no_more_good_{0};
  unsigned no_more_bad{0};
};
