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

	// uint256 test
	/*
	boost::multiprecision::uint256_t uint256 {"11579208923731619542357098500868790785326998466564056403945758400791312963993"};
	boost::multiprecision::uint256_t two = 2;

	std::cout << uint256 * two << std::endl;
	*/

	// Blockchain tests

	/*
	Blockchain b("voteChain v22.11", 1667732142);
	b.printLastBlock();

	for (unsigned int i = 0; i < 100; i++) {
		for (unsigned int j = 0; j < 10000; j++) {
			b.addTx(picosha2::hash256_hex_string(std::to_string(j)), picosha2::hash256_hex_string(std::to_string(i)));
		}
		b.addBlock();
	}

	*/

	// Mempool load from json + listening for votes from clients
	/*
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
	std::cout << "Result :" << std::endl;
	for(auto const &result : results)
		std::cout << result.first << ": " << result.second <<std::endl;

	Storage* storage = b.getStorage();
	// Test of func checkIfAddressVoted
	std::string address = "000c15d0ea8224c9649c9adee30c3030ee769b2b5f6d6594f94d446adf60396b";
	std::cout <<"Address : " + address + " voted: "  << storage->checkIfAddressVoted(address) << std::endl;
    node.close();
	mempool.close();
	t.join();
	*/

	// Count votes
	/*

	Storage storage(50);

	std::unordered_map<std::string, unsigned int> results;

	if(storage.countVotes(results)) {
		for(auto const &result : results)
			std::cout << result.first << ": " << result.second <<std::endl;
	}

	std::cout << storage.checkIfAddressVoted("000c15d0ea8224c9649c9adee30c3030ee769b2b5f6d6594f94d446adf60396b") << std::endl;
	*/

	return EXIT_SUCCESS;
}
