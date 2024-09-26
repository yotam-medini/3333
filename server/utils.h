// -*- c++ -*-
#if !defined(UTILS_H)
#define UTILS_H 1

#include <string>
#include <source_location>
#include <vector>

extern int GetTime();
extern int StrToInt(const std::string &s, int defval=(-1));
extern std::string dq(const std::string&);
extern std::string brace(const std::string&);
extern std::string Indent(const std::string &text, size_t n, bool pre=false);
extern std::vector<std::string> SSplit(const std::string &s);
extern std::string FuncName(
  const std::source_location location = std::source_location::current());
extern bool ValidateUnsigned(const std::string &s);
extern std::string YMDHMS();

#endif /* UTILS_H */
