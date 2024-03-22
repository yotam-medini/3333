// -*- c++ -*-
#if !defined(UTILS_H)
#define UTILS_H 1

#include <string>
#include <source_location>
#include <vector>

extern int GetTime();
extern std::string Indent(const std::string &text, size_t n);
extern std::vector<std::string> SSplit(const std::string &s);
extern std::string funcname(
  const std::source_location location = std::source_location::current());
extern bool validate_unsigned(const std::string &s);
extern std::string YMDHMS();

#endif /* UTILS_H */
