#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>
#include "merkle.h"
#include "blockchain.h"
#include "mempool.h"
#include "storage.h"
#include "node.h"
#include "..\..\lib\sha256_lib\picosha2.h"
#include "..\..\lib\json_lib\json.hpp"
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

	std::map<std::string, std::string> map;
	map.insert(std::pair<std::string, std::string>("A", "B"));
	map.insert(std::pair<std::string, std::string>("C", "D"));
	map.insert(std::pair<std::string, std::string>("E", "F"));
	map.insert(std::pair<std::string, std::string>("G", "H"));
	map.insert(std::pair<std::string, std::string>("I", "J"));
	map.insert(std::pair<std::string, std::string>("K", "L"));
	std::cout << merkle::get_merkle_root_hash(map);
	*/

	// Json test
	/*
	// create a map
	std::map<std::string, std::string> m3{ {"key", "val"}, {"key1", "val1"} };

	// create and print a JSON from the map
	nlohmann::json j1 = {
		{"id", 10},
		{"map", m3}
	};
	std::cout << j1 << std::endl;

	// get the map out of JSON
	std::map<std::string, std::string> m4 = j1["map"];
	// make sure the roundtrip succeeds
	assert(m3 == m4);
	*/

	/*
	nlohmann::json j = {
		{"pi", 3.141},
		{"happy", true},
		{"name", "Niels"},
		{"nothing", nullptr},
		{"answer", {
			{"everything", 42}
		}},
		{"list", {1, 0, 2}},
		{"object", {
			{"currency", "USD"},
			{"value", 42.99}
		}}
	};

	std::cout << j << std::endl;
	std::cout << "###########################" << std::endl;
	for(auto& i : j)
		std::cout << i << std::endl;
	std::cout << "###########################" << std::endl;
	// even easier with structured bindings (C++17)
	for (auto& [key, value] : j.items()) {
		std::cout << key << " : " << value << "\n";
	}
	*/

	/*
	nlohmann::json j2 = {};
	j2["list"] = { 1,2,3,4 };
	j2["list1"] = { 1,2,3,4 };
	j2["pi"] = 3.14159;
	j2["string"] = "test";

	for (auto& [key, value] : j2.items()) {
		std::cout << key << " : " << value << "\n";
	}
	*/

	// uint256 test
	/*
	boost::multiprecision::uint256_t uint256 {"11579208923731619542357098500868790785326998466564056403945758400791312963993"};
	boost::multiprecision::uint256_t two = 2;

	std::cout << uint256 * two << std::endl;
	*/

	// mixed tests
	/*
	nlohmann::json mixedJson = {};
	boost::multiprecision::uint256_t bigNumber {"11579208923731619542357098500868790785326998466564056403945758400791312963993"};
	boost::multiprecision::uint256_t bigNumber2 {"1456734561234562435756745768567845678456743567365487756484673"};
	boost::multiprecision::uint256_t bigNumber3 = bigNumber * bigNumber2;
	mixedJson["bigNumber"] = bigNumber.str();
	mixedJson["bigNumber2"] = bigNumber2.str();
	mixedJson["bigNumber3"] = bigNumber3.str();

	for (auto& [key, value] : mixedJson.items()) {
		std::cout << key << " : " << value << "\n";
	}

	std::string s = mixedJson.dump();
	std::cout << s << std::endl;
	s = picosha2::hash256_hex_string(s);
	std::cout << s << std::endl;
	*/

	// Blockchain tests
	/*
	// 0
	Blockchain b("voteChain v22.11", 1667732142);
	b.print();
	// 1
	b.addTx("A", "B");
	b.addTx("C", "D");
	b.addBlock();
	system("cls");
	b.print();
	// 2
	b.addTx("E", "F");
	b.addTx("G", "H");
	b.addBlock();
	system("cls");
	b.print();
	// 3
	b.setDifficulty(2);
	b.addTx("I", "J");
	b.addTx("K", "L");
	b.addBlock();
	system("cls");
	b.print();
	// 4
	b.setDifficulty(4);
	b.addTx("M", "N");
	b.addTx("O", "P");
	b.addTx("Q", "R");
	b.addTx("S", "T");
	b.addBlock();
	system("cls");
	b.print();
	// 5
	b.addTx("U", "V");
	b.addTx("W", "X");
	b.addTx("Y", "Z");
	b.addBlock();
	system("cls");
	b.print();
	*/
	
	/*
	Blockchain b("voteChain v22.11", 1667732142);
	b.printLastBlock();

	for (unsigned int i = 0; i < 100; i++) {
		for (unsigned int j = 0; j < 10000; j++) {
			b.addTx(picosha2::hash256_hex_string(std::to_string(j)), picosha2::hash256_hex_string(std::to_string(i)));			
		}
		b.addBlock();
		b.printLastBlock();
	}

	*/

	// Mempool tests
	/*
	// Create mempool and add some votes
	Mempool m(1667734373);
	m.addTx("A", "B");
	m.addTx("C", "D");
	m.addTx("E", "F");
	m.addTx("G", "H");
	m.addTx("I", "J");
	m.addTx("K", "L");
	m.addTx("M", "N");
	m.addTx("O", "P");
	m.addTx("Q", "R");
	m.addTx("S", "T");
	m.addTx("U", "V");
	m.addTx("W", "X");
	m.addTx("Y", "Z");
	m.print();

	std::cout << std::endl;
	std::cout << std::endl;

	// Get 6 votes from mempool to candidate block
	std::map<std::string, std::string> candidateBlockData;
	m.getTxs(candidateBlockData, 6);
	for (auto el : candidateBlockData) std::cout << el.first << ": " << el.second << std::endl;

	std::cout << std::endl;
	std::cout << std::endl;

	// Mempool should not have these 6 votes
	m.print();
	*/

	// storage test
	/*
	Blockchain b("voteChain v22.11", 1667732142);
	b.printLastBlock();

	for (unsigned int i = 0; i < 10; i++) {
		for (unsigned int j = 0; j < 10; j++) {
			b.addTx(picosha2::hash256_hex_string(std::to_string(j)), picosha2::hash256_hex_string(std::to_string(i)));
		}
		b.addBlock();
		b.printLastBlock();
	}
	b.addRemainingTxs();
	system("cls");
	
	Storage* s = b.getStorage();
	Block* blk = s->readBlockFromJson(10);

	auto data = blk->getData();

	assert(merkle::get_merkle_root_hash(data) == blk->getMerkleRoot());

	delete blk;

	b.printBlockchain();
	*/		
	
	// Node test
	//*

		
	//*/

	return EXIT_SUCCESS;
}
