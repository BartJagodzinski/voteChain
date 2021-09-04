#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
#include <string> 
#include <vector>
#include <tuple>
#include <ctime>

class Block {
private:
	unsigned int _id;
	std::time_t _timestamp;
	std::vector<std::tuple<std::string, std::string>> _data;
	std::string _prevHash;
	unsigned int _proof;
	std::string _hash;
public:
	Block(unsigned int id, std::time_t timestamp, std::vector<std::tuple<std::string, std::string>> data, std::string prevHash, unsigned int proof, std::string hash)
		: _id(id), _timestamp(timestamp), _data(data), _prevHash(prevHash), _proof(proof), _hash(hash) {}

	std::string	getHash() { return _hash; }
	unsigned int getId() { return _id; }
	unsigned int getProof() { return _proof; }

	void print() {
		std::cout << "id        : " << _id << std::endl;
		std::cout << "timestamp : " << _timestamp << std::endl;		
		std::cout << "proof     : " << _proof << std::endl;
		std::cout << "hash:     : " << _hash << std::endl;
		for (auto el : _data) {
			std::cout << "------------------------------------------" << std::endl;
			std::cout << "sender:  " << std::get<0>(el) << std::endl;
			std::cout << "reciver: " << std::get<1>(el) << std::endl;			
		}
	}
};

#endif

