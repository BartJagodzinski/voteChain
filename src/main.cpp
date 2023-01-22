#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
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


//*
	try {

		std::pair<std::string, unsigned short> mempoolIpPort;
		if(!config::getEndpointsFromJson(mempoolIpPort, "mempool_config.json", "mempool")) { return EXIT_FAILURE; }

		boost::asio::io_context ioContextMempool;
		boost::asio::ip::tcp::endpoint mempoolListenEndpoint(boost::asio::ip::address::from_string(mempoolIpPort.first), mempoolIpPort.second);

		Mempool mempool(ioContextMempool, mempoolListenEndpoint, std::time(nullptr)+30);
		std::thread mempoolThrd([&ioContextMempool](){ ioContextMempool.run(); });

		if(!mempool.loadVotesFromJson("mempool_load_test.json")) std::cerr << "Error in loading votes from json." << std::endl;

// ------------------------------------------------------------------------------------------------------------------------------------------------------

		std::pair<std::string, unsigned short> checkerIpPort;
		if(!config::getEndpointsFromJson(checkerIpPort, "checker_config.json", "checker")) { return EXIT_FAILURE; }
		boost::asio::io_context ioContextChecker;
		boost::asio::ip::tcp::endpoint checkerListenEndpoint(boost::asio::ip::address::from_string(checkerIpPort.first), checkerIpPort.second);

		Checker checker(ioContextChecker, checkerListenEndpoint);
		std::thread checkerThrd([&ioContextChecker](){ ioContextChecker.run(); });


// ------------------------------------------------------------------------------------------------------------------------------------------------------

		Blockchain blockchain("voteChain v23.1");
		blockchain.setDifficulty(4);

		while(mempool.isOpen() || !mempool.isEmpty()) {
			mempool.getVotes(blockchain.getData(), 10, blockchain.getLenght());
			blockchain.addBlock();
		}

		std::unordered_map<std::string, unsigned int> results;
		blockchain.countVotes(results);
		std::cout << "Result :" << std::endl;
		for(auto const &result : results)
			std::cout << result.first << ": " << result.second <<std::endl;

		mempool.close();
    	mempoolThrd.join();
		checker.close();
    	checkerThrd.join();
  	} catch (std::exception& e) { std::cerr << "Exception: " << e.what() << std::endl; return EXIT_FAILURE; }
	return EXIT_SUCCESS;
//*/
}
