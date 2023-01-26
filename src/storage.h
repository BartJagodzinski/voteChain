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
		if(votesToCheck.size() < 2) return true;
		std::unordered_map<std::string, std::string> votesToCheckCpy = votesToCheck;
		votesToCheck.clear();
		for(size_t id = 0; id <= lenght; ++id) {
			std::ifstream blockJsonFile("block_" + std::to_string(id) + ".json");
			if (!blockJsonFile)	{ std::cerr << "Error in reading block_"+std::to_string(id)+".json" << std::endl; return false; }
			nlohmann::ordered_json blockJson;
			blockJsonFile >> blockJson;
			blockJsonFile.close();
			std::unordered_map<std::string, std::string> votes = blockJson["votes"];
			//auto it = votesToCheckCpy.begin();
			for(auto it = votesToCheckCpy.begin(); it != votesToCheckCpy.end();) {
				if (votes.count(it->first) > 0) it = votesToCheckCpy.erase(it);
				else ++it;
			}
		}
		auto it = votesToCheckCpy.begin();
		for(auto it = votesToCheckCpy.begin(); it != votesToCheckCpy.end(); ++it)
			votesChecked.insert({it->first, it->second});
		return true;
	}

	// Return 0 if not voted, 1 if voted and -1 if error occurs.
	int checkIfAddressVoted(std::string address, size_t lenght) {
		for(size_t id = 1; id <= lenght; ++id) {
			std::ifstream blockJsonFile("block_" + std::to_string(id) + ".json");
			if (!blockJsonFile)	{ std::cerr << "Error in reading block_"+std::to_string(id)+".json" << std::endl; return -1; }
			nlohmann::ordered_json blockJson;
			blockJsonFile >> blockJson;
			std::map<std::string, std::string> votes = blockJson["votes"];
			blockJsonFile.close();
			if (votes.count(address) > 0) return 1;
		}
		return 0;
	}

	bool countVotes(std::unordered_map<std::string, unsigned int> &results, size_t lenght) {
		for(size_t id = 1; id <= lenght; ++id) {
			std::ifstream blockJsonFile("block_" + std::to_string(id) + ".json");
			if (!blockJsonFile)	{ std::cerr << "Error in reading block_"+std::to_string(id)+".json" << std::endl; return false; }
			nlohmann::ordered_json blockJson;
			blockJsonFile >> blockJson;
			std::map<std::string, std::string> votes = blockJson["votes"];
			blockJsonFile.close();

			for(auto const &vote : votes) {
				auto it = results.find(vote.second);
				if (it == results.end()) results.insert({vote.second, 1});
				if (it != results.end()) it->second += 1;
			}
		}
		return true;
	}

	Block* readBlockFromJson(size_t id) {
		std::string fileName = "block_" + std::to_string(id) + ".json";
		std::ifstream blockJsonFile(fileName);
		if (!blockJsonFile)	{ std::cerr << "Error in reading block_"+std::to_string(id)+".json" << std::endl; return nullptr; }

		nlohmann::ordered_json blockJson;
		blockJsonFile >> blockJson;

		boost::multiprecision::uint256_t nonce{ blockJson["nonce"].get<std::string>() };
		std::map<std::string, std::string> votes = blockJson["votes"];

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

		nlohmann::ordered_json blockJson = {
			{"id", block->getId()},
			{"timestamp", block->getTimestamp()},
			{"prevHash", block->getPrevHash()},
			{"merkleRoot", block->getMerkleRoot()},
			{"nonce", block->getNonce().str()},
			{"hash", block->getHash()},
			{"target", block->getTarget()},
			{"votes", block->getVotes()}
		};

		blockchainJson << std::setw(1) << blockJson << std::endl;
		blockchainJson.close();
	}
}

#endif
