#include "utils.h"
#include <ctime>
#include <fmt/core.h>

std::string dq(const std::string& s) {
  std::string ret;
  ret.reserve(s.size() + 2);
  ret.push_back('"');
  ret += s;
  ret.push_back('"');
  return ret;
}

std::string ymdhms(unsigned epcoh) {
  time_t t = epcoh;
  std::tm tm;
  gmtime_r(&t, &tm);
  std::string s = fmt::format("{:04d}/{:02d}/{:02d} {:02d}:{:02d}:{:02d}",
    tm.tm_year + 1900,(tm.tm_mon), tm.tm_mday,
    tm.tm_hour, tm.tm_min, tm.tm_sec);
  return s;
}
