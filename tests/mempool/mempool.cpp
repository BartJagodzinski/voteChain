#include <iostream>
#include <boost/asio.hpp>
#include "mempool.h"

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) { std::cerr << "Usage: ./mempool <listenPort>\n"; return EXIT_FAILURE; }

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::endpoint listenEndpoint(boost::asio::ip::tcp::v4(), std::atoi(argv[1]));

    Mempool mempool(io_context, listenEndpoint, 1667734373);
    io_context.run();

  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

  return EXIT_SUCCESS;
}
