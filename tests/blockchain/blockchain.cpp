#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include "merkle.h"
#include "blockchain.h"
#include "mempool.h"
#include "storage.h"
#include "node.h"
#include "picosha2.h"
#include "json.hpp"
#include <boost/multiprecision/cpp_int.hpp>

int main(int argc, char* argv[]) {  
  boost::asio::io_context io_context;

  boost::asio::ip::tcp::endpoint mempoolListenEndpoint(boost::asio::ip::tcp::v4(), 9876);
	// Start mempool with deadline 20s from now
  Mempool mempool(io_context, mempoolListenEndpoint, std::time(nullptr)+20);
	// Load sample votes from json file, votes can be added by starting client and sending vote from client side
	if(!mempool.loadVotesFromJson("mempool_load_test.json")) std::cout << "err in loading from json" << std::endl;

	boost::asio::ip::tcp::endpoint nodeListenEndpoint(boost::asio::ip::tcp::v4(), 8888);
  Node node(io_context, nodeListenEndpoint);
  boost::asio::ip::tcp::resolver resolver(io_context);

  std::thread t([&io_context](){ io_context.run(); });

	Blockchain b("voteChain v23.1");
	b.setDifficulty(4);
	std::unordered_map<std::string, unsigned int> results;

	// Hardcoded lock !mempool.isEmpty() to avoid generating empty blocks
	while(mempool.isOpen() && !mempool.isEmpty()) {
		mempool.getVotes(b.getData(), 3);
		b.addBlock();
	}
	b.countVotes(results);
	std::cout << "Results :" << std::endl;
	for(auto const &result : results)
		std::cout << result.first << ": " << result.second <<std::endl;

	Storage* storage = b.getStorage();
	// Test of func checkIfAddressVoted
	std::string address = "000c15d0ea8224c9649c9adee30c3030ee769b2b5f6d6594f94d446adf60396b";
	std::cout <<"Address : " + address + " voted: "  << storage->checkIfAddressVoted(address) << std::endl;
  node.close();
	mempool.close();
	t.join();

	return EXIT_SUCCESS;
}
