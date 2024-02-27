#include "utils.h"
#include <source_location>

std::string funcname() {
  std::source_location location = std::source_location::current();
  return location.function_name();
}
