#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <ctime>
#include <chrono>
#include "block.h"
#include "merkle.h"
#include "storage.h"
#include "picosha2.h"
#include <boost/multiprecision/cpp_int.hpp>

class Blockchain {
private:
	std::vector<Block*> _blockchain;
	std::unordered_map<std::string,std::string> _data;
	std::string _merkleRoot;
	std::time_t _timestamp;
	boost::multiprecision::uint256_t _nonce;
	std::string _hash;
	std::string _prevHash;
	unsigned int _lenght = 0;
	unsigned int _difficulty = 4;
	const std::string _target = "0000000000000000000000000000000000000000000000000000000000000000";
	Storage _storage = Storage(_lenght);
public:
	Blockchain(std::string blockZeroPrevHash) {
		std::cout << "Blockchain C'tor" << std::endl;
		_blockchain.reserve(_blockchain.size() + 1);
		_merkleRoot = merkle::get_merkle_root_hash(_data);
		_timestamp = std::time(nullptr);
		_prevHash = blockZeroPrevHash;
		findNonce(picosha2::hash256_hex_string(std::to_string(_lenght) + std::to_string(_timestamp) + _merkleRoot + _prevHash));
		_blockchain.push_back(new Block(_lenght, _timestamp, _prevHash, _merkleRoot, _nonce, _hash, _data));
		_storage.createSingleBlockJson(_blockchain.at(0));
		printLastBlock();
		_blockchain.at(0)->~Block();
		_blockchain.clear();
	}

	Storage* getStorage() { return &_storage; }
	std::unordered_map<std::string,std::string> &getData() { return _data; }

	void setDifficulty(unsigned int newDifficulty) { _difficulty = newDifficulty; }

	bool countVotes(std::unordered_map<std::string, unsigned int> &results) { return (_storage.countVotes(results)) ? true : false; }

	void hash() {
		_merkleRoot = merkle::get_merkle_root_hash(_data);
		findNonce(picosha2::hash256_hex_string(std::to_string(_lenght) + std::to_string(_timestamp) + _merkleRoot + _prevHash));
	}

	void findNonce(std::string const &toHash) {
		boost::multiprecision::uint256_t nonce = 0;
		std::string hash;
		while (true) {
			hash = toHash + nonce.str();
			hash = picosha2::hash256_hex_string(hash);
			if (hash.substr(0, _difficulty) == _target.substr(0, _difficulty)) break;
			++nonce;
		}
		_nonce = nonce;
		_hash = hash;
	}

	void addTx(std::string sender, std::string receiver) { _data.insert({sender, receiver}); }

	void addBlock() {
		_lenght++;
		_blockchain.reserve(_blockchain.size() + 1);
		_timestamp = std::time(nullptr);
		hash();
		_blockchain.push_back(new Block(_lenght, _timestamp, _prevHash, _merkleRoot, _nonce, _hash, _data));
		_storage.createSingleBlockJson(_blockchain.at(0));
		printLastBlock();
		_prevHash = _hash;
		_data.clear();
		_blockchain.at(0)->~Block();
		_blockchain.clear();
	}

	void printLastBlock() { std::cout << "\n########################\n" << std::endl; _blockchain.back()->printBlock(); }

	void addRemainingTxs() {
		while (_blockchain.size() > 0) {
			_storage.createSingleBlockJson(_blockchain.at(0));
			_blockchain.at(0)->~Block();
			_blockchain.erase(_blockchain.begin());
		}
	}

	void wipe() { for (auto el : _blockchain) el->~Block();	}

	~Blockchain() { wipe(); std::cout << "Blockchain D'tor" << std::endl; }
};

#endif