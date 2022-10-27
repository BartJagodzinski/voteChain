#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <ctime>
#include "block.h"
#include "merkle.h"
#include "..\..\lib\sha256_lib\picosha2.h"
#include <boost/multiprecision/cpp_int.hpp>

class Blockchain {
private:
	std::vector<Block> _blockchain;
	std::map<std::string,std::string> _data;
	std::string _merkleRoot;
	std::time_t _timestamp;
	boost::multiprecision::uint256_t _nonce;
	std::string _hash;
public:
	Blockchain(std::string blockZeroPrevHash) {
		std::cout << "Blockchain C'tor" << std::endl;
		_blockchain.reserve(_blockchain.size() + 1);
		_timestamp = std::time(nullptr);
		unsigned int id = 0;
		_merkleRoot = merkle::get_merkle_root_hash(_data);
		std::string toHash = std::to_string(id) + "," + std::to_string(_timestamp) + "," + _merkleRoot + "," + blockZeroPrevHash;
		validate_nonce(picosha2::hash256_hex_string(toHash));
		_blockchain.push_back(Block(id, _timestamp, blockZeroPrevHash, _merkleRoot, _nonce, _hash, _data));
	}

	void hash() {
		_merkleRoot = merkle::get_merkle_root_hash(_data);
		std::string toHash = std::to_string(_blockchain.size()) + "," + std::to_string(_timestamp) + "," + _merkleRoot + "," + _blockchain.back().getHash();
		validate_nonce(picosha2::hash256_hex_string(toHash));
	}

	void validate_nonce(std::string const toHash) {
		boost::multiprecision::uint256_t nonce = 0;
		std::string hash;
		while (true) {
			hash = toHash + nonce.str();
			hash = picosha2::hash256_hex_string(hash);
			if (hash.substr(0, 4) == "0000") break;
			nonce++;
		}
		_nonce = nonce;
		_hash = hash;
	}

	void addTx(std::string sender, std::string receiver) { _data.insert(std::pair<std::string, std::string>(sender, receiver));	}

	void addBlock() {
		_blockchain.reserve(_blockchain.size() + 1);
		_timestamp = std::time(nullptr);
		hash();
		_blockchain.push_back(Block(_blockchain.size(), _timestamp, _blockchain.back().getHash(), _merkleRoot, _nonce, _hash, _data));
		_data.clear();
	}

	void print() { for (auto el : _blockchain) { std::cout << "\n########################\n"<<std::endl; el.printBlock(); } }

	~Blockchain() { std::cout << "Blockchain D'tor" << std::endl; }
};

#endif
