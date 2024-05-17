#include "utils.h"
#include <charconv>
#include <ctime>
#include <source_location>
#include <sstream>
#include <fmt/core.h>

int GetTime() {
  return time(nullptr);
}

int StrToInt(const std::string &s, int defval) {
  int n;
  auto [_, ec] = std::from_chars(s.data(), s.data() + s.size(), n);
  if (ec != std::errc()) {
    n = defval;
  }
  return n;
}

std::string dq(const std::string &s) {
  return fmt::format(R"("{}")", s);
}

extern std::string brace(const std::string &s) {
  auto ret = fmt::format(" {} ", s);
  ret[0] = '{';
  ret[ret.size() - 1] = '}';
  return ret;
}

std::string Indent(const std::string &text, size_t n) {
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

std::vector<std::string> SSplit(const std::string &s) {
  std::vector<std::string> ret;
  std::stringstream ss{s};
  std::string w;
  while (ss >> w) {
    ret.emplace_back(w);
  }
  return ret;
}

std::string FuncName(const std::source_location location) {
  return location.function_name();
}

bool ValidateUnsigned(const std::string &s) {
  bool valid = !s.empty();
  if (valid) {
    if (! std::all_of(s.begin(), s.end(), isdigit)) {
      valid = (s.size() > 2) && (s[0] == '0') && (s[1] == 'x' || s[1] == 'X')
        && std::all_of(s.begin() + 2, s.end(), ::isxdigit);
    }
  }
  return valid;
}

std::string YMDHMS() {
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
