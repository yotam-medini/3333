#include "utils.h"
#include <ctime>
#include <source_location>
#include <sstream>

std::string funcname(const std::source_location location) {
  return location.function_name();
}

std::string ymdhms() {
  auto s2 = [] (int n) -> std::string {
    std::ostringstream oss2;
    if (n < 10) { oss2 << '0'; }
    oss2 << n;
    return oss2.str();
  };
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << tm.tm_year + 1900 << '/' << s2(tm.tm_mon) << '/' << s2(tm.tm_mday) <<
    ' ' << s2(tm.tm_hour) << ':' << s2(tm.tm_min) << ':' << s2(tm.tm_sec);
  return oss.str();
}
