#ifndef STORAGE_H
#define STORAGE_H
#include <iostream>
#include <fstream>
#include <string>
#include "block.h"
#include "json.hpp"
#include <boost/multiprecision/cpp_int.hpp>

namespace storage {

	bool checkVotes(std::unordered_map<std::string, std::string> &votesToCheck, std::unordered_map<std::string, std::string> &votesChecked, size_t lenght) {
		for(size_t id = 1; id <= lenght; ++id) {
			std::ifstream blockJsonFile("block_" + std::to_string(id) + ".json");
			if (!blockJsonFile)	return false;
			nlohmann::json blockJson;
			blockJsonFile >> blockJson;
			std::unordered_map<std::string, std::string> votes = blockJson["votes"];
			blockJsonFile.close();
			for (auto it = votesToCheck.begin(); it != votesToCheck.end(); ++it)
				if (votes.count(it->first) > 0) votesToCheck.erase(it);
		}
		auto it = votesToCheck.begin();
		while (it != votesToCheck.end()) {
			votesChecked.insert({it->first, it->second});
			it = votesToCheck.erase(it);
		}
		return true;
	}

	// Return 0 if not voted, 1 if voted and -1 if error occurs.
	int checkIfAddressVoted(std::string address, size_t lenght) {
		for(size_t id = 1; id <= lenght; ++id) {
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

	bool countVotes(std::unordered_map<std::string, unsigned int> &results, size_t lenght) {
		for(size_t id = 1; id <= lenght; ++id) {
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

		boost::multiprecision::uint256_t nonce{ blockJson["nonce"].get<std::string>() };
		std::unordered_map<std::string, std::string> votes = blockJson["votes"];

		Block* block = new Block
			(id,
			blockJson["timestamp"].get<std::time_t>(),
			blockJson["prevHash"].get<std::string>(),
			blockJson["merkleRoot"].get<std::string>(),
			nonce,
			blockJson["hash"].get<std::string>(),
			blockJson["target"].get<unsigned short>(),
			votes);

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
			{"target", block->getTarget()},
			{"votes", block->getData()}
		};

		blockchainJson << std::setw(1) << blockJson << std::endl;
		blockchainJson.close();
	}
}

#endif
