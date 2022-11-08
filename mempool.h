#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include "..\..\lib\json_lib\json.hpp"
#include <boost/multiprecision/cpp_int.hpp>

class Mempool {
private:
	std::map<std::string,std::string> _data;
	std::time_t _deadline;
public:
	Mempool(std::time_t deadline) : _deadline(deadline) { std::cout << "Mempool C'tor" << std::endl; }

	void getTxs(std::map<std::string, std::string> &candidateBlockData, unsigned int nTxToRemove) {
		auto it = _data.begin();
		// Both must be true to avoid removing more txs than are in mempool
		while (it != _data.end() && nTxToRemove > 0) {
			candidateBlockData.insert(std::pair<std::string, std::string>(it->first, it->second));
			it = _data.erase(it);
			--nTxToRemove;
		}
	}

	void print() { for (auto el : _data) std::cout << el.first << ": " << el.second << std::endl; }

	void addTx(std::string sender, std::string receiver) { _data.insert(std::pair<std::string, std::string>(sender, receiver)); }

	~Mempool() { std::cout << "Mempool D'tor" << std::endl; }
};



#endif
