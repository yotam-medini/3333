#include "logger.h"
#include <filesystem>
#include <fstream>
#include <fmt/core.h>
#include "utils.h"

class Logger::Impl {
 public:
  Impl(
    const std::string &filename,
    const std::string &filename_backup,
    size_t line_size=1000,
    size_t flush_period=10) :
    filename_(filename),
    filename_backup_(filename_backup),
    line_size_(line_size),
    flush_period_(flush_period),
    line_count_(0) {
    Cycle();
  }
  void log(const std::string &line) {
    ofs_ << fmt::format("{} {}\n", YMDHMS(), line);
    ++line_count_;
    if (line_count_ >= line_size_) {
      ofs_.close();
      Cycle();
      line_count_ = 0;
    } else if (line_count_ % flush_period_ == 0) {
      ofs_.flush();
    }
  }
 private:
  void Cycle() {
    if (std::filesystem::exists(filename_)) {
      std::filesystem::rename(filename_, filename_backup_);
    }
    ofs_.open(filename_);
  }
  const std::string filename_;
  const std::string filename_backup_;
  const size_t line_size_;
  const size_t flush_period_;
  size_t line_count_;
  std::ofstream ofs_;
};

Logger::Logger(
  const std::string &filename,
  const std::string &filename_backup,
  size_t line_size,
  size_t flush_period) :
  pImpl_{std::make_unique<Impl>(
    filename, filename_backup, line_size, flush_period)} {
}

Logger::~Logger() {
}

void Logger::log(const std::string& line) {
  pImpl_->log(line);
}
