#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>
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

		std::pair<std::string, unsigned short> mempoolIpPort;
		if(!config::getEndpointsFromJson(mempoolIpPort, "mempool_config.json", "mempool")) { return EXIT_FAILURE; }

		boost::asio::io_context ioContextMempool;
		boost::asio::ip::tcp::endpoint mempoolListenEndpoint(boost::asio::ip::address::from_string(mempoolIpPort.first), mempoolIpPort.second);
		std::time_t deadline;
		if(!config::getDeadlineFromJson(deadline, "mempool_config.json", "mempool")) { return EXIT_FAILURE; }
		Mempool mempool(ioContextMempool, mempoolListenEndpoint, deadline);
		std::thread mempoolThrd([&ioContextMempool](){ ioContextMempool.run(); });

		std::string blockchainName;
		if(!config::getBlockchainNameFromJson(blockchainName, "mempool_config.json", "mempool")) { return EXIT_FAILURE; }
		Blockchain blockchain(blockchainName);
		unsigned short difficulty;
		if(!config::getDifficultyFromJson(difficulty, "mempool_config.json", "mempool")) { return EXIT_FAILURE; }
		blockchain.setDifficulty(difficulty);

		while(mempool.isOpen() || !mempool.isEmpty()) {
			mempool.getVotes(blockchain.getVotes(), 10, blockchain.getLenght());
			blockchain.addBlock();
		}

		if(!mempool.votesToCheckEmpty()) {
			mempool.getVotes(blockchain.getVotes(), 10, blockchain.getLenght());
			mempool.addLastVote(blockchain.getVotes(), blockchain.getLenght());
			blockchain.addBlock();
		}

		mempool.close();
    		mempoolThrd.join();
		if(!blockchain.verifyBlockchain()) { std::cerr << "Blockchain is invalid." <<std::endl; return EXIT_FAILURE; }

		std::unordered_map<std::string, unsigned int> results;
		storage::countVotes(results, blockchain.getLenght());
		std::cout << "Result :" << std::endl;
		for(auto const &result : results)
			std::cout << result.first << ": " << result.second <<std::endl;

  	} catch (std::exception& e) { std::cerr << "Exception: " << e.what() << std::endl; return EXIT_FAILURE; }
	return EXIT_SUCCESS;

}
