#ifndef SECURITY_H
#define SECURITY_H
#include <fstream>
#include <cstdio>

namespace security {

    void deleteWhitelistFile() {
        std::ofstream overwriteWhitelist("whitelist.json");
        overwriteWhitelist << "";
        overwriteWhitelist.close();
        std::remove("whitelist.json");
    }
}
#endif
