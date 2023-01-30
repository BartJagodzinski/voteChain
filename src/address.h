#ifndef ADDRESS_H
#define ADDRESS_H
#include "convert.h"
#include "base58.h"
#include "keys.h"

namespace address {

    bool getAddress(std::string &address, std::string &email, std::string &password, std::string &name, std::string &surname, std::string &id) {
        // If uncompressed key is needed change to keys::pub_key_uncompressed_size
        std::vector<uint8_t> pubKey(keys::pub_key_compressed_size);
        std::vector<uint8_t> secKey(picosha2::k_digest_size);

        keys::privateKeyFromHash(secKey, email+password+name+surname+id);

        // If uncompressed key is needed keys::publicKeyFromSecKey(pubKey, secKey, false)
        if(!keys::publicKeyFromSecKey(pubKey, secKey)) { std::cerr << "Can't create public key from given input." << std::endl; return false; }

        std::vector<uint8_t> pubKeyHash(picosha2::k_digest_size);
        // Double hash of pubKey
        picosha2::hash256(convert::uint8_to_hex_string(pubKey.data(), pubKey.size()), pubKeyHash);
        picosha2::hash256(convert::uint8_to_hex_string(pubKeyHash.data(), pubKeyHash.size()), pubKeyHash);

        // Checksum, '0' in front of hash to get first char of address: '1'
        pubKeyHash.insert(pubKeyHash.begin(), 0);

        std::vector<uint8_t> pubKeyChecksumHash(picosha2::k_digest_size);
        picosha2::hash256(convert::uint8_to_hex_string(pubKeyHash.data(), pubKeyHash.size()), pubKeyChecksumHash);
        picosha2::hash256(convert::uint8_to_hex_string(pubKeyChecksumHash.data(), pubKeyChecksumHash.size()), pubKeyChecksumHash);

        // Append first 8 char of checksum to pubKeyHash
        for(size_t i = 0; i < 4; ++i)
            pubKeyHash.push_back(pubKeyChecksumHash[i]);

        // Base58() to get address
        address = base58::encode(pubKeyHash, base58::map);
        return true;
    }
}
#endif
