#include <iostream>
#include <boost/asio.hpp>
#include "checker.h"

int main(int argc, char* argv[]) {
  try {
    //if (argc < 2) { std::cerr << "Usage: ./checker" << std::endl; return EXIT_FAILURE; }

    boost::asio::io_context io_context;
    std::pair<std::string, unsigned short> checkerIpPort;
    if(!config::getEndpointsFromJson(checkerIpPort, "checker_config.json", "checker")) { exit; }
    boost::asio::ip::tcp::endpoint listenEndpoint(boost::asio::ip::address::from_string(checkerIpPort.first), checkerIpPort.second);

    Checker checker(io_context, listenEndpoint);
    io_context.run();

  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

  return EXIT_SUCCESS;
}