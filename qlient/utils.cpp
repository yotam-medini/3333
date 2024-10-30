#include "utils.h"

std::string dq(const std::string& s) {
  std::string ret;
  ret.reserve(s.size() + 2);
  ret.push_back('"');
  ret += s;
  ret.push_back('"');
  return ret;
}

  
