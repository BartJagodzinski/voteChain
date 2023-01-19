#include <iostream>
#include <boost/asio.hpp>
#include "mempool.h"
#include "config.h"

int main(int argc, char* argv[]) {
  try {
    //if (argc < 2) { std::cerr << "Usage: ./mempool <listenPort>\n"; return EXIT_FAILURE; }


    std::pair<std::string, unsigned short> mempoolIpPort;
    if(!config::getEndpointsFromJson(mempoolIpPort, "mempool_config.json", "mempool")) { return EXIT_FAILURE; }

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::endpoint listenEndpoint(boost::asio::ip::address::from_string(mempoolIpPort.first), mempoolIpPort.second);

    Mempool mempool(io_context, listenEndpoint, std::time(nullptr)+600);
    io_context.run();
    if(!mempool.loadVotesFromJson("mempool_load_test.json")) std::cout << "err in loading from json" << std::endl;

  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

  return EXIT_SUCCESS;
}