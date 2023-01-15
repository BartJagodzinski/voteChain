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
	std::string &_prevHash;
public:
	Storage(unsigned int &lenght, std::string &prevHash) : _lenght(lenght), _prevHash(prevHash) { std::cout << "Storage C'tor" << std::endl; };

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
		unsigned short target = blockJson["target"];
		std::unordered_map<std::string, std::string> votes = blockJson["votes"];
		Block* block = new Block(id, timestamp, prevHash, merkleRoot, nonce, hash, target, votes);
		blockJsonFile.close();

		return block;
	}

	void createSingleBlockJson(Block* block) {
		std::string fileName = "block_" + std::to_string(block->getId()) + ".json";
		std::ofstream blockFile(fileName);

		nlohmann::json blockJson = {
			{"id", block->getId()},
			{"timestamp", block->getTimestamp()},
			{"prevHash", block->getPrevHash()},
			{"merkleRoot", block->getMerkleRoot()},
			{"nonce", block->getNonce().str()},
			{"hash", block->getHash()},
			{"target", block->getTarget()},
			{"votes", block->getData()}
		};

		//blockFile << std::setw(2) << blockJson << std::endl;
		blockFile << blockJson << std::endl;
		blockFile.close();
	}

	std::set<std::string, bool> verifyBlock(Block* block) {
		std::set<std::string, bool> status;
		if(block->getPrevHash() != _prevHash) status.insert({"prevHash", false});
		else status.insert({"prevHash", true});

		if(merkle::get_merkle_root_hash(block->getData()) != block->getMerkleRoot()) status.insert({"merkleRoot", false});
		else status.insert({"merkleRoot", true});

		if(picosha2::hash256_hex_string(std::to_string(block->getId()) + std::to_string(block->getTimestamp())
			+ block->getMerkleRoot() + block->getPrevHash() + block->getNonce().str()) != block->getHash()) status.insert({"hash", false});
		else status.insert({"hash", true});

		return status;
	}
	~Storage() { std::cout << "Storage D'tor" << std::endl; };
};


#endif