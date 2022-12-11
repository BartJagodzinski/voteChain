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
	unsigned int _id;
	std::time_t _timestamp;
	std::string _prevHash;
	std::string _merkleRoot;
	boost::multiprecision::uint256_t _nonce;
	std::string _hash;
	std::unordered_map<std::string,std::string> _data;
public:
	Block(unsigned int id, std::time_t timestamp, std::string prevHash, std::string merkleRoot, boost::multiprecision::uint256_t nonce, std::string hash, std::unordered_map<std::string, std::string> &data)
		: _id(id), _timestamp(timestamp), _prevHash(prevHash), _merkleRoot(merkleRoot), _nonce(nonce), _hash(hash), _data(data) { std::cout << "Block C'tor" << std::endl;	}

	unsigned int getId() const { return _id; }
	std::string getIdStr() const { return std::to_string(_id); }
	std::time_t getTimestamp() const { return _timestamp; }
	std::string getPrevHash() const { return _prevHash; }
	std::string getMerkleRoot() const { return _merkleRoot; }
	boost::multiprecision::uint256_t getNonce() const { return _nonce; }
	std::string getHash() const { return _hash; }
	const std::unordered_map<std::string, std::string> &getData() { return _data; }

	~Block() { std::cout << "Block D'tor" << std::endl; }
};

#endif
