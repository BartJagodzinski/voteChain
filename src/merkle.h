#ifndef MERKLE_H
#define MERKLE_H
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "picosha2.h"
#include <boost/multiprecision/cpp_int.hpp>

namespace merkle {

    std::vector<std::string> hashUntilRoot(std::vector<std::string> const &hashes) {
        std::vector<std::string> merkleTree;

        for (size_t pos = 0; pos + 1 < hashes.size(); pos += 2)
            merkleTree.push_back(picosha2::double_hash256_hex_string(hashes.at(pos) + hashes.at(pos + 1)));

        return merkleTree;
    }
    template <typename T>
    std::string getMerkleRootHash(T const& votes) {
        if (votes.size() == 0) return "empty";
        if (votes.size() == 1) {
            auto it = votes.begin();
            return picosha2::double_hash256_hex_string(it->first + it->second);
        }
        std::vector<std::string> hashes, merkleTree;
        std::string oddHash;
        bool odd = false;

        // Make vec of hashes from <string sender, string receiver> votes
        for (auto& [key, value] : votes)
            hashes.push_back(picosha2::double_hash256_hex_string(key + value));

        // If number of hashes is odd, store last hash into variable to make vec of hashes even
        if (hashes.size() % 2 != 0) {
            odd = true;
            oddHash = hashes.at(hashes.size() - 1);
            hashes.pop_back();
        }

        // Hash until root of even vector<hash>
        do {
            merkleTree = hashUntilRoot(hashes);
            hashes = merkleTree;
        } while (merkleTree.size() > 1);

        // If number of hashes was odd, hash received root of even vec<hash> with stored last odd hash
        if (odd) { return picosha2::double_hash256_hex_string(merkleTree.at(0) + oddHash); }

        return merkleTree.at(0);
    }
}

#endif
