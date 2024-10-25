#include "player.h"

Player::Player() {
}

void Player::reset() {
  set_calls_good = 0;
  set_calls_bad = 0;
  add3_good_ = 0;
  add3_bad = 0;
  no_more_good_ = 0;
  no_more_bad = 0;
}
