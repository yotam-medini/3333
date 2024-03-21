#include "utils.h"
#include <ctime>
#include <source_location>
#include <sstream>

std::string indent(const std::string &text, size_t n) {
  const std::string spaces(n, ' ');
  std::string ret;
  for (char c: text) {
    ret.push_back(c);
    if (c == '\n') {
      ret += spaces;
    }
  }
  return ret;
}

std::vector<std::string> ssplit(const std::string &s) {
  std::vector<std::string> ret;
  std::stringstream ss{s};
  std::string w;
  while (ss >> w) {
    ret.emplace_back(w);
  }
  return ret;
}

std::string funcname(const std::source_location location) {
  return location.function_name();
}

bool validate_unsigned(const std::string &s) {
  bool valid = !s.empty();
  if (valid) {
    if (! std::all_of(s.begin(), s.end(), isdigit)) {
      valid = (s.size() > 2) && (s[0] == '0') && (s[1] == 'x' || s[1] == 'X')
        && std::all_of(s.begin() + 2, s.end(), ::isxdigit);
    }
  }
  return valid;
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
