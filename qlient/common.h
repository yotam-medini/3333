#pragma once

#include <string>
#include <functional>

using new_table_func_t =
  std::function<int(
    const std::string&, // table & owner name
    const std::string&, // table password
    const std::string&) // owner password
  >;

using join_table_func_t =
  std::function<int(
    const std::string&, // table name
    const std::string&, // player name
    const std::string&, // table password
    const std::string&) // player password
  >;

    
