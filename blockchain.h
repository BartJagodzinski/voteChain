#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <iostream>
#include <string> 
#include <vector>
#include <tuple>
#include <ctime>
#include "block.h"
#include "picosha2.h"

class Blockchain {
private:
	std::vector<Block> _blockchain;
	std::vector<std::tuple<std::string, std::string>> _data;
	std::time_t _timestamp;
	unsigned int _proof;
	std::string _hash;
public:
	Blockchain() {
		_blockchain.reserve(_blockchain.size() + 1);
		std::string id = std::to_string(1);
		std::time_t time = std::time(nullptr);
		std::string timestamp = std::to_string(time);
		std::string votes;
		for (auto el : _data) { votes += " | " + std::get<0>(el) + " -> " + std::get<1>(el); }
		std::string prevHash = "840b76115ab41fbc6ff6b0e8c3181e43dfc925d086815c6bfc17a86115b677a0";
		std::string proof = std::to_string(100);
		std::string hash = id + "," + timestamp + "," + votes + "," + prevHash + "," + proof;
		hash = picosha2::hash256_hex_string(hash);
		_blockchain.push_back(Block(1, time, _data, "840b76115ab41fbc6ff6b0e8c3181e43dfc925d086815c6bfc17a86115b677a0", 100, hash));
	}

	void addVote(std::string sender, std::string recipient) { _data.reserve(_data.size() + 1); _data.push_back(std::make_tuple(sender, recipient)); }

	void hash() {
		std::string id = std::to_string(_blockchain.back().getId()+1);
		std::string timestamp = std::to_string(_timestamp);
		std::string votes;
		for (auto el : _data) {	votes += " | " + std::get<0>(el) + " -> " + std::get<1>(el); }
		std::string prevHash = _blockchain.back().getHash();
		std::string proof = std::to_string(_proof);
		std::string hash = id + "," + timestamp + "," + votes + "," + prevHash + "," + proof;
		_hash = picosha2::hash256_hex_string(hash);
	}

	void validate_proof() {
		unsigned int proof = 0;
		while (true) {
			unsigned int h = _blockchain.back().getProof() * proof;
			std::string str = std::to_string(h);
			std::string hex_str = picosha2::hash256_hex_string(str);
			if (hex_str.substr(0, 4) == "0000") break;
			proof++;
		}
		_proof = proof;
	}

	void addBlock() {
		_blockchain.reserve(_blockchain.size() + 1);
		_timestamp = std::time(nullptr);
		validate_proof();
		hash();
		_blockchain.push_back(Block(_blockchain.size() + 1, _timestamp, _data, _blockchain.back().getHash(), _proof, _hash));
		_data.clear();
	}

	void print() { for (auto el : _blockchain) { std::cout << "\n########################\n"<<std::endl; el.print(); } }
};

#endif
