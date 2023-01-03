#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include "merkle.h"
#include "blockchain.h"
#include "mempool.h"
#include "storage.h"
#include "node.h"
#include "picosha2.h"
#include "json.hpp"
#include <boost/multiprecision/cpp_int.hpp>

int main(int argc, char* argv[]) {
  Storage storage(50);

	std::unordered_map<std::string, unsigned int> results = storage.countVotes();

	for(auto const &result : results)
		std::cout << result.first << ": " << result.second <<std::endl;

	return EXIT_SUCCESS;
}
