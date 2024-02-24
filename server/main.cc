#include <iostream>
#include <boost/program_options.hpp>

#include "server.h"

int main(int argc, char **argv) {
  namespace po = boost::program_options;
  int rc = 0;
  po::variables_map vm;
  po::options_description desc("3333-server options");
  desc.add_options()
    ("help", "produce help message")
    ("host", po::value<std::string>()->default_value("localhost"),
      "Host address")
    ("maxtables", po::value<int>()->default_value(24), "Max tables")
    ("maxplayers", po::value<int>()->default_value(4), "Max Players")
    ("expire", po::value<int>()->default_value(300),
      "Seconds of no action to expire")
    ("pidfn", po::value<std::string>()->default_value("/tmp/3333-server.pid"),
      "File to record PID")
    ("debug", po::value<unsigned>()->default_value(0), "Debug flags")
  ;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << "\n";
  } else {
    std::cerr << "host=" << vm["host"].as<std::string>() << '\n';
    Server server{
       vm["host"].as<std::string>(),
       vm["maxtables"].as<size_t>(),
       vm["maxplayers"].as<size_t>(),
       vm["expire"].as<unsigned>(),
       vm["pidfn"].as<std::string>(),
       vm["debug"].as<unsigned>()};       
    server.run();
  }

  return rc;
}
