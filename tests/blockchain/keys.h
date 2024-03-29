#ifndef KEYS
#define KEYS
#include <iostream>
#include <secp256k1.h>
#include <stdio.h>
#include <string.h>
#include "picosha2.h"
#include <iomanip>
static secp256k1_context *ctx = NULL;

namespace keys {

    void printHex(unsigned char* data, size_t size) {
        for(size_t i = 0; i < size; i++)
            printf("%02x", data[i]);
        std::cout << std::endl;
    }

    bool publicKeyFromSecKey(std::vector<unsigned char> &pubKeyVec, std::vector<unsigned char> &secKey, bool compressed=true) {
        ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
        if(secp256k1_ec_seckey_verify(ctx, secKey.data())) {
            secp256k1_pubkey pubkey;
            size_t len = (compressed) ? 33 : 65;
            if(secp256k1_ec_pubkey_create(ctx, &pubkey, secKey.data())) {
                if(compressed) secp256k1_ec_pubkey_serialize(ctx, pubKeyVec.data(), &len, &pubkey, SECP256K1_EC_COMPRESSED);
                else secp256k1_ec_pubkey_serialize(ctx, pubKeyVec.data(), &len, &pubkey, SECP256K1_EC_UNCOMPRESSED);
                if(secp256k1_ec_pubkey_parse(ctx, &pubkey, pubKeyVec.data(), len)) {
                    secp256k1_context_destroy(ctx);
                    return true;
                }
            }
        }

        secp256k1_context_destroy(ctx);
        return false;
    }

    std::string hashesFromData(std::string mail, std::string password, std::string name, std::string surname, unsigned long long identificationNumber){
        return picosha2::hash256_hex_string(mail)+picosha2::hash256_hex_string(password)+picosha2::hash256_hex_string(name)+picosha2::hash256_hex_string(surname)+picosha2::hash256_hex_string(std::to_string(identificationNumber));
    }

    // std::vector<unsigned char> secKey(picosha2::k_digest_size);
    void privateKeyFromHash(std::vector<unsigned char> &secKey, std::string hash) { picosha2::hash256(hash, secKey); }
}

#endif
