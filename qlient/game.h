#pragma once

#include <vector>
#include "player.h"

class Game {
 public:
  Game();
  void reset();
  bool active_{false};
  int tstate_{-1};
  int gstate_{-1};
  int deck_{-1};
  std::vector<unsigned> cards_active_; // into 0..3^4-1 [0..81)
  std::vector<unsigned> cards_selected_; // into cards_active_
  std::vector<Player> players_;
};
