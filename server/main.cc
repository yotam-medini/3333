#include <cstdint>
#include <iostream>
#include <boost/program_options.hpp>

#include "server.h"
#include "utils.h"

int main(int argc, char **argv) {
  namespace po = boost::program_options;
  int rc = 0;
  po::variables_map vm;
  po::options_description desc("3333-server options");
  desc.add_options()
    ("help", "produce help message")
    ("host", po::value<std::string>()->default_value("127.0.0.1"),
      "Host address")
    ("port", po::value<uint16_t>()->default_value(9090), "Port")
    ("maxtables", po::value<size_t>()->default_value(24), "Max tables")
    ("maxplayers", po::value<size_t>()->default_value(4), "Max Players")
    ("expire", po::value<unsigned>()->default_value(300),
      "Seconds of no action to expire")
    ("pidfn", po::value<std::string>()->default_value("/tmp/3333-server.pid"),
      "File to record PID")
    ("debug", po::value<std::string>()->default_value("0"), "Debug flags")
  ;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << "\n";
  } else {
    std::cerr << "host=" << vm["host"].as<std::string>() << '\n';
    const std::string debug_flags_raw = vm["debug"].as<std::string>();
    if (!validate_unsigned(debug_flags_raw)) {
      std::cerr << "Bad debug value=" << debug_flags_raw << '\n';
      rc = 1;
    } else {
      unsigned debug_flags = std::stoi(debug_flags_raw, nullptr, 0);
      std::cerr << "debug_flags=" << debug_flags << '\n';
      Server server{
         vm["host"].as<std::string>(),
         vm["port"].as<uint16_t>(),
         vm["maxtables"].as<size_t>(),
         vm["maxplayers"].as<size_t>(),
         vm["expire"].as<unsigned>(),
         vm["pidfn"].as<std::string>(),
         debug_flags};       
      server.run();
    }
  }

  return rc;
}
