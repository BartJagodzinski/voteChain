#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>
#include "unordered_set_hash.h"
#include "picosha2.h"
#include "json.hpp"
#include "node_config.h"
#include "node.h"

int main(int argc, char* argv[]) {
    try {
        boost::asio::io_context io_context;
        // Server part of node
        boost::asio::ip::tcp::endpoint listenEndpoint(boost::asio::ip::address::from_string(config::getThisNodeIpPort().first), config::getThisNodeIpPort().second);

        Node node(io_context, listenEndpoint);
        boost::asio::ip::tcp::resolver resolver(io_context);

        std::thread t([&io_context](){ io_context.run(); });

        char line[2];
        while (std::cin.getline(line, 4)) {
            if(line[0] == '1') {
                node.broadcastBlock(1);
            }
        }

        node.close();
        t.join();
    } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

    return EXIT_SUCCESS;
}