#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_set>
#include "json.hpp"
#include "picosha2.h"
#include "unordered_set_hash.h"

namespace config {

    bool getEndpointsFromJson(std::pair<std::string, unsigned short> &ipPort, std::string fileName, std::string endpointName) {
        std::ifstream configFile(fileName);
        configFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!configFile || configFile.tellg() == 0) { std::cerr << "Error in " + endpointName + " config file" << std::endl; return false;}
        configFile.seekg(0, std::ios::beg);
        nlohmann::json configJson;
        configFile >> configJson;
        configFile.close();
        ipPort = configJson[endpointName].get<std::pair<std::string, unsigned short>>();
        return true;
    }

    bool getWhitelistFromJson(std::unordered_set<std::tuple<std::string, std::string, std::string, std::string, std::string>, UnorderedSetHashTuple<std::string>> &whitelist) {
        std::ifstream whitelistFile("whitelist.json");
        whitelistFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!whitelistFile || whitelistFile.tellg() == 0) { std::cerr << "Error in whitelist file" << std::endl; return false;}
        whitelistFile.seekg(0, std::ios::beg);
        nlohmann::json whitelistJson;
        whitelistFile >> whitelistJson;
        whitelistFile.close();
        for(auto const &voter : whitelistJson["voters"])
            whitelist.insert(std::make_tuple(voter["email"], voter["password"], voter["name"], voter["surname"], voter["id"]));
        return true;
    }
}

#endif