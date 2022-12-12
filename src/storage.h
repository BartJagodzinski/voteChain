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
		std::ofstream headerOnlyBlockchainJson("headerOnlyBlockchainJson.json", std::ios::app);
		blockchainJson << "[" << std::endl;
		headerOnlyBlockchainJson << "[" << std::endl;
		blockchainJson.close();
		headerOnlyBlockchainJson.close();
		std::cout << "Storage C'tor" << std::endl; 
	};

	Block* readBlockFromJson(unsigned int id) {
		std::string fileName = "block_" + std::to_string(id) + "_Json" + ".json";
		std::ifstream blockJsonFile(fileName);
		if (!blockJsonFile)	return nullptr;

		nlohmann::json blockJson;
		blockJsonFile >> blockJson;
		std::time_t timestamp = blockJson["timestamp"];
		std::string prevHash = blockJson["prevHash"];
		std::string merkleRoot = blockJson["merkleRoot"];
		boost::multiprecision::uint256_t nonce{ blockJson["nonce"].get<std::string>() };
		std::string hash = blockJson["hash"];
		std::unordered_map<std::string, std::string> votes = blockJson["votes"];
		Block* block = new Block(id, timestamp, prevHash, merkleRoot, nonce, hash, votes);
		blockJsonFile.close();

		return block;
	}

	void createSingleBlockJson(Block* block) {
		std::string fileName = "block_" + std::to_string(block->getId()) + "_Json" + ".json";
		std::ofstream blockchainJson(fileName, std::ios::app);
				
		nlohmann::json blockJson = {
			{"id", block->getId()},
			{"timestamp", block->getTimestamp()},
			{"prevHash", block->getPrevHash()},
			{"merkleRoot", block->getMerkleRoot()},
			{"nonce", block->getNonce().str()},
			{"hash", block->getHash()},
			{"votes", block->getData()}
		};	

		blockchainJson << std::setw(2) << blockJson << std::endl;
		blockchainJson.close();
	}

	void appendBlock(Block *block, char c) {
		std::ofstream blockchainJson("blockchainJson.json", std::ios::app);
		std::ofstream headerOnlyBlockchainJson("headerOnlyBlockchainJson.json", std::ios::app);

		nlohmann::json blockJson = {
			{"id", block->getId()},
			{"timestamp", block->getTimestamp()},
			{"prevHash", block->getPrevHash()},
			{"merkleRoot", block->getMerkleRoot()},
			{"nonce", block->getNonce().str()},
			{"hash", block->getHash()}
		};
		
		headerOnlyBlockchainJson << std::setw(2) << blockJson << c << std::endl;
		headerOnlyBlockchainJson.close();

		blockJson["votes"] = block->getData();
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

	void close() {
		std::ofstream blockchainJson("blockchainJson.json", std::ios::app);
		std::ofstream headerOnlyBlockchainJson("headerOnlyBlockchainJson.json", std::ios::app);
		nlohmann::json endOfBlockchain = { {"id", "end"} };
		blockchainJson << std::setw(2) << endOfBlockchain << "]" << std::endl;
		headerOnlyBlockchainJson << std::setw(2) << endOfBlockchain << "]" << std::endl;
		blockchainJson.close();
		headerOnlyBlockchainJson.close();
	}

	~Storage() { std::cout << "Storage D'tor" << std::endl; };
};


#endif
