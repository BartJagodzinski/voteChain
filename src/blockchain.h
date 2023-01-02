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
	std::time_t _deadline;
	unsigned int _difficulty = 2;
	const std::string _target = "0000000000000000000000000000000000000000000000000000000000000000";
	unsigned int _slidingWindowSize = 5;
	Storage _storage = Storage();
public:
	Blockchain(std::string blockZeroPrevHash, std::time_t deadline) {
		std::cout << "Blockchain C'tor" << std::endl;
		_deadline = deadline;
		_blockchain.reserve(_blockchain.size() + 1);	
		unsigned int id = 0;
		_merkleRoot = merkle::get_merkle_root_hash(_data);
		_timestamp = std::time(nullptr);
		std::string toHash = std::to_string(id) + "," + std::to_string(_timestamp) + "," + _merkleRoot + "," + blockZeroPrevHash;
		findNonce(picosha2::hash256_hex_string(toHash));
		_blockchain.push_back(new Block(id, _timestamp, blockZeroPrevHash, _merkleRoot, _nonce, _hash, _data));
	}

	std::time_t getDeadline() const { return _deadline; }
	Storage* getStorage() { return &_storage; }

	void setDifficulty(unsigned int newDifficulty) { _difficulty = newDifficulty; }
	void setSlidingWindowSize(unsigned int newSize) { if(newSize > 0) _slidingWindowSize = newSize; }

	void hash() {
		_merkleRoot = merkle::get_merkle_root_hash(_data);
		std::string toHash = std::to_string(_blockchain.size()) + "," + std::to_string(_timestamp) + "," + _merkleRoot + "," + _blockchain.back()->getHash();
		findNonce(picosha2::hash256_hex_string(toHash));
	}

	void findNonce(std::string const &toHash) {
		boost::multiprecision::uint256_t nonce = 0;
		std::string hash;
		while (true) {
			hash = toHash + nonce.str();
			hash = picosha2::hash256_hex_string(hash);
			if (hash.substr(0, _difficulty) == _target.substr(0, _difficulty)) break;
			nonce++;
		}
		_nonce = nonce;
		_hash = hash;
	}

	void addTx(std::string sender, std::string receiver) { _data.insert({sender, receiver}); }

	void slidingWindow() {
		if (_blockchain.size() > _slidingWindowSize) {
			_storage.appendBlock(_blockchain.at(0), ',');
			_storage.createSingleBlockJson(_blockchain.at(0));
			_blockchain.at(0)->~Block();
			_blockchain.erase(_blockchain.begin());
		}
	}

	void addBlock() {		
		_blockchain.reserve(_blockchain.size() + 1);
		_timestamp = std::time(nullptr);
		hash();
		_blockchain.push_back(new Block(_blockchain.back()->getId() + 1, _timestamp, _blockchain.back()->getHash(), _merkleRoot, _nonce, _hash, _data));
		_data.clear();
		slidingWindow();
	}

	void printBlockchain() { _storage.printBlockchain(); }
	void printLastBlock() { std::cout << "\n########################\n" << std::endl; _blockchain.back()->printBlock(); }
		
	void addRemainingTxs() {
		while (_blockchain.size() > 0) {
			_storage.appendBlock(_blockchain.at(0), ',');
			_storage.createSingleBlockJson(_blockchain.at(0));
			_blockchain.at(0)->~Block();
			_blockchain.erase(_blockchain.begin());
		}
		_storage.close();
	}

	void wipe() { for (auto el : _blockchain) el->~Block();	}

	~Blockchain() { wipe(); std::cout << "Blockchain D'tor" << std::endl; }
};

#endif
