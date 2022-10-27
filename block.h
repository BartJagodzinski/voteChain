#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <ctime>
#include <boost/multiprecision/cpp_int.hpp>

class Block {
private:
// <header>
	unsigned int _id;
	std::time_t _timestamp;
	std::string _prevHash;
	std::string _merkleRoot;
// </header>
	boost::multiprecision::uint256_t _nonce;
	std::string _hash;
	std::map<std::string,std::string> _data;
public:
	Block(unsigned int id, std::time_t timestamp, std::string prevHash, std::string merkleRoot, boost::multiprecision::uint256_t nonce, std::string hash, std::map<std::string, std::string> &data)
		: _id(id), _timestamp(timestamp), _prevHash(prevHash), _merkleRoot(merkleRoot), _nonce(nonce), _hash(hash), _data(data) {}

	unsigned int getId() { return _id; }
	std::time_t getTimestamp() { return _timestamp; }
	std::string getPrevHash() { return _prevHash; }
	std::string getMerkleRoot() { return _merkleRoot; }
	boost::multiprecision::uint256_t getNonce() { return _nonce; }
	std::string	getHash() { return _hash; }

	void printBlock() {
		std::cout << "id         : " << _id << std::endl;
		std::cout << "timestamp  : " << _timestamp << std::endl;
		std::cout << "prevHash   : " << _prevHash << std::endl;
		std::cout << "merkleRoot : " << _merkleRoot << std::endl;
		std::cout << "nonce      : " << _nonce << std::endl;
		std::cout << "hash:      : " << _hash << std::endl;
		for (auto &el : _data) {
			std::cout << "------------------------------------------" << std::endl;
			std::cout << "sender:  " << el.first << std::endl;
			std::cout << "receiver: " << el.second << std::endl;
		}
	}
};

#endif
