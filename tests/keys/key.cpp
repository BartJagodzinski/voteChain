#include "keys.h"

int main() {
    // If uncompressed key is needed change 33 to 65
    std::vector<unsigned char> pubKey(33);
    std::vector<unsigned char> secKey(picosha2::k_digest_size);

    keys::privateKeyFromHash(secKey, keys::hashesFromData("mail", "pass", "name", "surname", 99999));

    // If uncompressed key is needed keys::publicKeyFromSecKey(pubKey, secKey, false)
    if(keys::publicKeyFromSecKey(pubKey, secKey)) std::cout << "Success" << std::endl;

    std::cout << "Secret Key: "; keys::printHex(secKey.data(), secKey.size());
    std::cout << "Public Key: "; keys::printHex(pubKey.data(), pubKey.size());
    return EXIT_SUCCESS;
}
