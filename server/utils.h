// -*- c++ -*-
#if !defined(UTILS_H)
#define UTILS_H 1

#include <string>
#include <source_location>
#include <vector>

extern std::string indent(const std::string &text, size_t n);
extern std::vector<std::string> ssplit(const std::string &s);
extern std::string funcname(
  const std::source_location location = std::source_location::current());
extern bool validate_unsigned(const std::string &s);
extern std::string ymdhms();

#endif /* UTILS_H */
