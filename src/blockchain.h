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
#include "mempool.h"
#include "picosha2.h"
#include <boost/multiprecision/cpp_int.hpp>

class Blockchain {
private:
	std::vector<Block*> _blockchain;
	std::map<std::string, std::string> _votes;
	std::string _merkleRoot;
	std::time_t _timestamp;
	boost::multiprecision::uint256_t _nonce;
	std::string _hash;
	std::string _prevHash;
	std::string _blockZeroPrevHash;
	size_t _lenght = 0;
	unsigned short _difficulty = 4;
	const std::string _target = "00000000000000000000";
public:
	Blockchain(std::string blockZeroPrevHash) {
		std::cout << "Blockchain C'tor" << std::endl;
		_blockchain.reserve(_blockchain.size() + 1);
		_blockZeroPrevHash = blockZeroPrevHash;
		_prevHash = blockZeroPrevHash;
		hash();
		_blockchain.push_back(new Block(_lenght, _timestamp, _prevHash, _merkleRoot, _nonce, _hash, _difficulty, _votes));
		_prevHash = _blockchain.at(0)->getHash();
		storage::createSingleBlockJson(_blockchain.at(0));
		printLastBlock();
		delete _blockchain.at(0);
		_blockchain.clear();
	}

	std::map<std::string,std::string> &getVotes() { return _votes; }

	size_t getLenght() const { return _lenght; }

	void setDifficulty(unsigned short newDifficulty) { _difficulty = (newDifficulty <= 20) ? newDifficulty : _difficulty; }

	bool verifyBlock(size_t id, std::string &prevHash) {
		const std::string _target = "00000000000000000000";
		Block *block = storage::readBlockFromJson(id);

		if(block->getPrevHash() != prevHash) return false;
		if(block->getId() != id) return false;
		if(block->getMerkleRoot() != merkle::getMerkleRootHash(block->getVotes())) return false;
		std::string hash = std::to_string(block->getId()) + std::to_string(block->getTimestamp()) + block->getPrevHash() + block->getMerkleRoot() + block->getNonce().str();
		hash = picosha2::hash256_hex_string(hash);
		if(block->getHash() != hash) return false;
		if(block->getHash().substr(0, block->getTarget()) != _target.substr(0, block->getTarget())) return false;
		prevHash = block->getHash();

		delete block;
		return true;
	}

	bool verifyBlockchain() {
		std::string prevHash = _blockZeroPrevHash;
		for(size_t id = 0; id <= _lenght; ++id)
			if(!verifyBlock(id, prevHash)) return false;
		return true;
	}

	void hash() {
		_merkleRoot = merkle::getMerkleRootHash(_votes);
		_timestamp = std::time(nullptr);
		std::string toHash = std::to_string(_lenght) + std::to_string(_timestamp) + _prevHash + _merkleRoot;
		boost::multiprecision::uint256_t nonce = 0;
		std::string hash;
		do {
			++nonce;
			hash = toHash + nonce.str();
			hash = picosha2::hash256_hex_string(hash);
    	} while (hash.substr(0, _difficulty) != _target.substr(0, _difficulty));
		_nonce = nonce;
		_hash = hash;
	}

	void addBlock() {
		++_lenght;
		_blockchain.reserve(_blockchain.size() + 1);
		hash();
		_blockchain.push_back(new Block(_lenght, _timestamp, _prevHash, _merkleRoot, _nonce, _hash, _difficulty, _votes));
		storage::createSingleBlockJson(_blockchain.at(0));
		printLastBlock();
		_prevHash = _hash;
		_votes.clear();
		delete _blockchain.at(0);
		_blockchain.clear();
	}

	void printLastBlock() { std::cout << "\n########################\n" << std::endl; _blockchain.back()->printBlock(); }

	void wipe() { for (auto el : _blockchain) delete el; }

	~Blockchain() { wipe(); std::cout << "Blockchain D'tor" << std::endl; }
};

#endif
