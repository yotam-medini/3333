#include "utils.h"
#include <source_location>

std::string funcname(const std::source_location location) {
  return location.function_name();
}
