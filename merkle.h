#ifndef MERKLE_H
#define MERKLE_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <memory>
#include "..\..\lib\sha256_lib\picosha2.h"
#include <boost/multiprecision/cpp_int.hpp>

namespace merkle {

    std::vector<std::string> hash_until_root(std::vector<std::string> const &hashes) {
        std::vector<std::string> merkleTree;
        
        for (size_t pos = 0; pos + 1 < hashes.size(); pos += 2)
            merkleTree.push_back(picosha2::double_hash256_hex_string(hashes.at(pos) + hashes.at(pos + 1)));

        return merkleTree;
    }

    template <class T>
    bool isEmpty(T const& data) {
        int it = 0;
        for (auto& [key, value] : data) {
            it++;
            if (it > 0) return false;
        }
        if (it == 0) return true;
    }

    template <class T>
    std::string get_merkle_root_hash(T const& data) {
        if (data.size() == 0) return "empty";
        std::vector<std::string> hashes, merkleTree;

        // make vec of hashes from <string sender, string receiver> data
        for (auto& [key, value] : data)
            hashes.push_back(picosha2::double_hash256_hex_string(key + value));

        do {
            // if number of hashes is odd, copy last hash as its neighbour
            if (hashes.size() % 2 != 0) hashes.push_back(hashes.at(hashes.size() - 1));
            merkleTree = hash_until_root(hashes);
            hashes = merkleTree;
        } while (merkleTree.size() > 1);

        return merkleTree.at(0);
    }
}

#endif
