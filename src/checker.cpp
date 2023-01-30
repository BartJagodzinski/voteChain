#include <unordered_map>
#include <iostream>
#include <fstream>
#include <ctime>
#include <boost/multiprecision/cpp_int.hpp>
#include "blockchain.h"
#include "picosha2.h"
#include "checker.h"
#include "mempool.h"
#include "storage.h"
#include "merkle.h"
#include "json.hpp"
#include "config.h"

int main(int argc, char* argv[]) {

	try {
        std::pair<std::string, unsigned short> checkerIpPort;
		if(!config::getEndpointsFromJson(checkerIpPort, "checker_config.json", "checker")) { return EXIT_FAILURE; }
		boost::asio::io_context ioContextChecker;
		boost::asio::ip::tcp::endpoint checkerListenEndpoint(boost::asio::ip::address::from_string(checkerIpPort.first), checkerIpPort.second);

        std::time_t deadline;
        if(!config::getDeadlineFromJson(deadline, "checker_config.json", "checker")) { return EXIT_FAILURE; }

		Checker checker(ioContextChecker, checkerListenEndpoint, deadline);
		std::thread checkerThrd([&ioContextChecker](){ ioContextChecker.run(); });

        while(checker.isOpen()) { /* wait for deadline */ }

        checker.close();
    	checkerThrd.join();
    } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << std::endl; return EXIT_FAILURE; }

	return EXIT_SUCCESS;
}
