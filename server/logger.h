// -*- c++ -*-
#if !defined(LOGGER_H)
#define LOGGER_H 1

#include <memory>
#include <string>

class Logger {
 public:
  Logger(
    const std::string &filename,
    const std::string &filename_backup,
    size_t line_size=1000,
    size_t flush_period=1); // use larger for relase !!!!!!!!!!!!!!!!!!!!!!!!!
  void log(const std::string &line);
  ~Logger();
 private:
  class Impl;
  std::unique_ptr<Impl> pImpl_;
};

#endif /* LOGGER_H */
