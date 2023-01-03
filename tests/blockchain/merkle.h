#ifndef MERKLE_H
#define MERKLE_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <memory>
#include "picosha2.h"
#include <boost/multiprecision/cpp_int.hpp>

namespace merkle {

    std::vector<std::string> hash_until_root(std::vector<std::string> const &hashes) {
        std::vector<std::string> merkleTree;
        
        for (size_t pos = 0; pos + 1 < hashes.size(); pos += 2)
            merkleTree.push_back(picosha2::double_hash256_hex_string(hashes.at(pos) + hashes.at(pos + 1)));

        return merkleTree;
    }

    template <class T>
    std::string get_merkle_root_hash(T const& data) {
        if (data.size() == 0) return "empty";
        std::vector<std::string> hashes, merkleTree;
        std::string oddHash;
        bool odd = false;

        // Make vec of hashes from <string sender, string receiver> data
        for (auto& [key, value] : data)
            hashes.push_back(picosha2::double_hash256_hex_string(key + value));

        // If number of hashes is odd, store last hash into variable to vec of hashes make it even
        if (hashes.size() % 2 != 0) {
            odd = true;
            oddHash = hashes.at(hashes.size() - 1);
            hashes.pop_back();
        }

        // Hash until root of even vector<hash>
        do {
            merkleTree = hash_until_root(hashes);
            hashes = merkleTree;
        } while (merkleTree.size() > 1);

        // If number of hashes was odd, hash received root of even vec<hash> with stored last odd hash 
        if (odd) { return picosha2::double_hash256_hex_string(merkleTree.at(0) + oddHash); }

        return merkleTree.at(0);
    }
}

#endif
