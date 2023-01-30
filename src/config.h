#ifndef CONFIG_H
#define CONFIG_H
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "json.hpp"
#include "picosha2.h"
#include "unordered_set_hash.h"

namespace config {

    bool getEndpointsFromJson(std::pair<std::string, unsigned short> &ipPort, std::string fileName, std::string endpointName) {
        std::ifstream configFile(fileName);
        configFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!configFile || configFile.tellg() == 0) { std::cerr << "Error in " + endpointName + " config file." << std::endl; return false;}
        configFile.seekg(0, std::ios::beg);
        nlohmann::json configJson;
        configFile >> configJson;
        configFile.close();
        auto config = configJson[endpointName].at(0);
        ipPort = std::make_pair(config["ip"], config["port"].get<unsigned short>());
        return true;
    }

    bool getDeadlineFromJson(std::time_t &deadline, std::string fileName, std::string podName) {
        std::ifstream configFile(fileName);
        configFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!configFile || configFile.tellg() == 0) { std::cerr << "Error in " + podName + " config file." << std::endl; return false;}
        configFile.seekg(0, std::ios::beg);
        nlohmann::json configJson;
        configFile >> configJson;
        configFile.close();
        auto config = configJson[podName].at(0);
        deadline = config["deadline"].get<std::time_t>();
        return true;
    }

    bool getDifficultyFromJson(unsigned short &difficulty, std::string fileName, std::string podName) {
        std::ifstream configFile(fileName);
        configFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!configFile || configFile.tellg() == 0) { std::cerr << "Error in " + podName + " config file." << std::endl; return false;}
        configFile.seekg(0, std::ios::beg);
        nlohmann::json configJson;
        configFile >> configJson;
        configFile.close();
        auto config = configJson[podName].at(0);
        difficulty = config["difficulty"];
        return true;
    }

    bool getBlockchainNameFromJson(std::string &name, std::string fileName, std::string podName) {
        std::ifstream configFile(fileName);
        configFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!configFile || configFile.tellg() == 0) { std::cerr << "Error in " + podName + " config file." << std::endl; return false;}
        configFile.seekg(0, std::ios::beg);
        nlohmann::json configJson;
        configFile >> configJson;
        configFile.close();
        auto config = configJson[podName].at(0);
        name = config["name"];
        return true;
    }

    bool getWhitelistAndMempoolEndpoint(std::vector<char> &buff, std::unordered_set<std::tuple<std::string, std::string, std::string, std::string, std::string>, UnorderedSetHashTuple<std::string>> &whitelist, std::pair<std::string, unsigned short> &ipPort) {
        std::string msgFromChecker(buff.begin(), buff.end());
        if(msgFromChecker.size() == 0) { std::cerr << "Error in message from checker. " << std::endl; return false; }
        auto msgJson = nlohmann::json::parse(msgFromChecker);
        for(auto const &voter : msgJson["voters"])
            whitelist.insert(std::make_tuple(voter["email"], voter["password"], voter["name"], voter["surname"], voter["id"]));
        auto mempoolEndpoint = msgJson["mempool"].at(0);
        ipPort = std::make_pair(mempoolEndpoint["ip"], mempoolEndpoint["port"].get<unsigned short>());
        return true;
    }

    bool getCandidatesFromJson(std::unordered_set<std::pair<std::string, std::string>, UnorderedSetHashPair<std::string, std::string>> &candidates) {
        std::ifstream candidatesFile("candidates.json");
        candidatesFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!candidatesFile || candidatesFile.tellg() == 0) { std::cerr << "Error in candidates file." << std::endl; return false;}
        candidatesFile.seekg(0, std::ios::beg);
        nlohmann::json candidatesJson;
        candidatesFile >> candidatesJson;
        candidatesFile.close();
        for(auto const &candidate : candidatesJson["candidates"])
            candidates.insert(std::make_pair(candidate["name"], candidate["address"]));
        return true;
    }

}

#endif
