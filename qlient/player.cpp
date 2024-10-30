#include "player.h"

Player::Player() {
}

void Player::reset() {
  set_calls_good_ = 0;
  set_calls_bad_ = 0;
  add3_good_ = 0;
  add3_bad_ = 0;
  no_more_good_ = 0;
  no_more_bad_ = 0;
}

int Player::score() const {
  int good = set_calls_good_ + add3_good_ + no_more_good_;
  int bad = set_calls_bad_ + add3_bad_ + no_more_bad_;
  return good - bad;
}

