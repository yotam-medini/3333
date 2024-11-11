#include "player.h"
#include <fmt/core.h>
#include "utils.h"

Player::Player(const std::string& name, const std::string &password) :
  name_(name),
  password_(password),
  tcreated_(GetTime()) {
}

void Player::SetTAction() {
  taction_ = GetTime();
}

void Player::RestNumbers() {
  sets_found_ = 0;
  bad_calls_ = 0;
  add3s_good_ = 0;
  add3s_bad_ = 0;
  no_more_good_ = 0;
  no_more_bad_ = 0;
}

std::string Player::json() const {
  std::string j{"{\n"};
  j += fmt::format(R"j(  "name": "{}",)j" "\n", name_);
  j += fmt::format(R"j(  "say": "{}",)j" "\n", say_);
  j += fmt::format(R"j(  "numbers": [{}, {}, {}, {}, {}, {}],)j" "\n",
    sets_found_,
    bad_calls_,
    add3s_good_,
    add3s_bad_,
    no_more_good_,
    no_more_bad_);
  j += fmt::format(R"j(  "tcreated": {},)j" "\n", tcreated_);
  j += fmt::format(R"j(  "taction": {})j" "\n", taction_);
  j += "}";
  return j;
}
