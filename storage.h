#ifndef STORAGE_H
#define STORAGE_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <ctime>
#include <chrono>
#include "block.h"
#include "blockchain.h"
#include "merkle.h"
#include "mempool.h"
#include "..\..\lib\sha256_lib\picosha2.h"
#include "..\..\lib\json_lib\json.hpp"
#include <boost/multiprecision/cpp_int.hpp>

class Storage {
private:

public:
	Storage() { 
		std::ofstream blockchainJson("blockchainJson.json", std::ios::app);
		blockchainJson << "[" << std::endl;
		blockchainJson.close();
		std::cout << "Storage C'tor" << std::endl; 
	};

	void appendBlock(Block *block, char c) {		
		std::ofstream blockchainJson("blockchainJson.json", std::ios::app);
		nlohmann::json blockJson = {
			{"id", block->getId()},
			{"timestamp", block->getTimestamp()},
			{"prevHash", block->getPrevHash()},
			{"merkleRoot", block->getMerkleRoot()},
			{"nonce", block->getNonce().str()},
			{"hash", block->getHash()},
			{"votes", {block->getData()}}
		};		
		blockchainJson << std::setw(2) << blockJson << c << std::endl;
		blockchainJson.close();
	}

	void printBlockchain() {
		std::ifstream blockchainJson("blockchainJson.json");
		if (!blockchainJson) std::cout << "Blockchain is empty." << std::endl;
		
		nlohmann::json blockchain;
		blockchainJson >> blockchain;

		for (auto& [key, value] : blockchain.items())
			std::cout << key << " : " << value << "\n";

		blockchainJson.close();
	}

	~Storage() { 
		std::ofstream blockchainJson("blockchainJson.json", std::ios::app);
		nlohmann::json endOfBlockchain = {{"id", "end"}};
		blockchainJson << std::setw(2) << endOfBlockchain << "]" << std::endl;
		blockchainJson.close();
		std::cout << "Storage D'tor" << std::endl; 
	};
};


#endif
