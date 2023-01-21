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

	// Merkle root hash test
	/*
	std::vector<std::tuple<std::string, std::string>> somedata;
	somedata.push_back(std::make_tuple("A", "B"));
	somedata.push_back(std::make_tuple("C", "D"));
	somedata.push_back(std::make_tuple("E", "F"));
	somedata.push_back(std::make_tuple("G", "H"));
	somedata.push_back(std::make_tuple("I", "J"));
	somedata.push_back(std::make_tuple("K", "L"));
	std::cout << merkle::get_merkle_root_hash(somedata) << std::endl;

	std::unordered_map<std::string, std::string> map;
	map.insert(std::pair<std::string, std::string>("A", "B"));
	map.insert(std::pair<std::string, std::string>("C", "D"));
	map.insert(std::pair<std::string, std::string>("E", "F"));
	map.insert(std::pair<std::string, std::string>("G", "H"));
	map.insert(std::pair<std::string, std::string>("I", "J"));
	map.insert(std::pair<std::string, std::string>("K", "L"));
	std::cout << merkle::get_merkle_root_hash(map);
	*/


	// Mempool load from json + listening for votes from clients
//*
	try {

		std::pair<std::string, unsigned short> mempoolIpPort;
		if(!config::getEndpointsFromJson(mempoolIpPort, "mempool_config.json", "mempool")) { return EXIT_FAILURE; }

		boost::asio::io_context ioContextMempool;
		boost::asio::ip::tcp::endpoint mempoolListenEndpoint(boost::asio::ip::address::from_string(mempoolIpPort.first), mempoolIpPort.second);

		Mempool mempool(ioContextMempool, mempoolListenEndpoint, std::time(nullptr)+500);
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
		blockchain.setDifficulty(5);

		while(mempool.isOpen() || mempool.isEmpty()) {
			mempool.getVotes(blockchain.getData(), 10, blockchain.getLenght());
			blockchain.addBlock();
		}

		std::unordered_map<std::string, unsigned int> results;
		blockchain.countVotes(results);
		std::cout << "Result :" << std::endl;
		for(auto const &result : results)
			std::cout << result.first << ": " << result.second <<std::endl;

		mempool.close();
		checker.close();
    	mempoolThrd.join();
    	checkerThrd.join();

  	} catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }
//*/


	return EXIT_SUCCESS;
}
