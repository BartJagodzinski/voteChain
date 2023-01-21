#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <ctime>
#include <boost/multiprecision/cpp_int.hpp>

class Block {
private:
	size_t _id;
	std::time_t _timestamp;
	std::string _prevHash;
	std::string _merkleRoot;
	boost::multiprecision::uint256_t _nonce;
	std::string _hash;
	unsigned short _target;
	std::unordered_map<std::string,std::string> _data;
public:
	Block(unsigned int id, std::time_t timestamp, std::string prevHash, std::string merkleRoot, boost::multiprecision::uint256_t nonce, std::string hash, unsigned short target, std::unordered_map<std::string, std::string> &data)
	: _id(id), _timestamp(timestamp), _prevHash(prevHash), _merkleRoot(merkleRoot), _nonce(nonce), _hash(hash), _target(target), _data(data)
	{ std::cout << "Block C'tor" << std::endl; }

	unsigned int getId() const { return _id; }
	std::time_t getTimestamp() const { return _timestamp; }
	std::string getPrevHash() const { return _prevHash; }
	std::string getMerkleRoot() const { return _merkleRoot; }
	boost::multiprecision::uint256_t getNonce() const { return _nonce; }
	std::string getHash() const { return _hash; }
	unsigned short getTarget() const { return _target; }
	const std::unordered_map<std::string, std::string> &getData() { return _data; }

	void printBlock() {
		std::cout << "id         : " << _id << std::endl;
		std::cout << "timestamp  : " << _timestamp << std::endl;
		std::cout << "prevHash   : " << _prevHash << std::endl;
		std::cout << "merkleRoot : " << _merkleRoot << std::endl;
		std::cout << "nonce      : " << _nonce << std::endl;
		std::cout << "hash       : " << _hash << std::endl;
		std::cout << "target     : " << _target << std::endl;
	}

	~Block() { std::cout << "Block D'tor" << std::endl; }
};

#endif
