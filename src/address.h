#ifndef ADDRESS_H
#define ADDRESS_H
#include "keys.h"
#include "base58.h"
#include "convert.h"
#include "ripemd160.h"

namespace address {

    bool getAddress(std::string &address, std::string &email, std::string &password, std::string &name, std::string &surname, std::string &id) {
        // If uncompressed key is needed change to keys::pub_key_uncompressed_size
        std::vector<uint8_t> pubKey(keys::pub_key_compressed_size);
        std::vector<uint8_t> secKey(picosha2::k_digest_size);

        keys::privateKeyFromHash(secKey, picosha2::hash256_hex_string(email+password+name+surname+id));

        // If uncompressed key is needed keys::publicKeyFromSecKey(pubKey, secKey, false)
        if(!keys::publicKeyFromSecKey(pubKey, secKey)) { std::cerr << "Can't create public key from given input." << std::endl; return false; }

        std::vector<uint8_t> pubKeyHash(picosha2::k_digest_size);
        picosha2::hash256(convert::uint8_to_hex_string(pubKey.data(), pubKey.size()), pubKeyHash);

        // If uncompressed key is needed change to keys::pub_key_uncompressed_size
        uint8_t pubKeyHashArr[keys::pub_key_compressed_size];
        uint8_t ripemd160HashArr[ripemd160::k_digest_size];

        // Copy pubKey into array to pass it into ripemd160::hash()
        std::copy(pubKeyHash.begin(), pubKeyHash.end(), pubKeyHashArr);

        ripemd160::hash(pubKeyHashArr, picosha2::k_digest_size, ripemd160HashArr);
        memset(pubKeyHashArr, 0, sizeof(pubKeyHashArr));

        // Copy ripemd160 hash into vector
        std::vector<uint8_t> ripemd160Hash(ripemd160HashArr, ripemd160HashArr + ripemd160::k_digest_size);
        memset(ripemd160HashArr, 0, sizeof(ripemd160HashArr));

        // Checksum of ripemd160 hash
        // append '0' in front of hash to get first char of address: '1'
        ripemd160Hash.insert(ripemd160Hash.begin(), 0);

        // SHA256(SHA256(ripemd160)) = 'h'
        std::vector<uint8_t> ripemd160DoubleSha256(picosha2::k_digest_size);
        picosha2::hash256(convert::uint8_to_hex_string(ripemd160Hash.data(), ripemd160Hash.size()), ripemd160DoubleSha256);
        picosha2::hash256(convert::uint8_to_hex_string(ripemd160DoubleSha256.data(), ripemd160DoubleSha256.size()), ripemd160DoubleSha256);

        // Append first 8 char of 'h' to ripemd160 hash
        for(size_t i = 0; i < 4; ++i)
            ripemd160Hash.push_back(ripemd160DoubleSha256[i]);

        // Base58() to get address
        address = base58::encode(ripemd160Hash, base58::map);
        return true;
    }
}
#endif
