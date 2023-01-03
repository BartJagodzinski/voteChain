#ifndef STORAGE_H
#define STORAGE_H
#include <iostream>
#include <fstream>
#include <string>
#include "block.h"
#include "blockchain.h"
#include "merkle.h"
#include "mempool.h"
#include "picosha2.h"
#include "json.hpp"
#include <boost/multiprecision/cpp_int.hpp>

class Storage {
private:
	unsigned int &_lenght;
public:
	Storage(unsigned int &lenght) : _lenght(lenght) { std::cout << "Storage C'tor" << std::endl; };
	unsigned int getLenght() { return _lenght; }
	// return 0 if not voted, 1 if voted and -1 if error occurs.
	int checkIfAddressVoted(std::string address) {
		for(size_t id = 1; id <= _lenght; ++id) {
			std::ifstream blockJsonFile("block_" + std::to_string(id) + ".json");
			if (!blockJsonFile)	return -1;
			nlohmann::json blockJson;
			blockJsonFile >> blockJson;
			std::unordered_map<std::string, std::string> votes = blockJson["votes"];
			blockJsonFile.close();
			if (votes.find(address) != votes.end()) return 1;
		}
		return 0;
	}

	bool countVotes(std::unordered_map<std::string, unsigned int> &results) {
		for(size_t id = 1; id <= _lenght; ++id) {
			std::ifstream blockJsonFile("block_" + std::to_string(id) + ".json");
			if (!blockJsonFile)	return false;
			nlohmann::json blockJson;
			blockJsonFile >> blockJson;
			std::unordered_map<std::string, std::string> votes = blockJson["votes"];
			blockJsonFile.close();

			for(auto const &vote : votes) {
				auto it = results.find(vote.second);
				if (it == results.end()) results.insert({vote.second, 1});
				if (it != results.end()) it->second += 1;
			}
		}
		return true;
	}

	Block* readBlockFromJson(unsigned int id) {
		std::string fileName = "block_" + std::to_string(id) + ".json";
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
		std::string fileName = "block_" + std::to_string(block->getId()) + ".json";
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

	~Storage() { std::cout << "Storage D'tor" << std::endl; };
};


#endif