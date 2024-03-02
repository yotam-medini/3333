// -*- c++ -*-
#if !defined(UTILS_H)
#define UTILS_H 1

#include <string>
#include <source_location>

extern std::string funcname(
  const std::source_location location = std::source_location::current());

extern std::string ymdhms();

#endif /* UTILS_H */
