#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H
#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
#include "picosha2.h"

namespace config {

    std::pair<std::string, unsigned short> getThisNodeIpPort() {
        std::ifstream configFile("node_config.json");
        configFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!configFile || configFile.tellg() == 0) { std::cerr << "Error in config file" << std::endl; exit;}
        configFile.seekg(0, std::ios::beg);
        nlohmann::json configJson;
        configFile >> configJson;
        configFile.close();
        std::pair<std::string, unsigned short> ipPort = configJson["node"].get<std::pair<std::string, unsigned short>>();
        return ipPort;
    }

    bool getAllNodesFromJson(std::unordered_map<std::string, std::pair<std::string, unsigned short>> &nodes) {
        std::ifstream nodesFile("nodes.json");
        nodesFile.seekg(0,  std::ios::end);
        // If error in opening or file empty
        if (!nodesFile || nodesFile.tellg() == 0) return false;
        nodesFile.seekg(0, std::ios::beg);
        nlohmann::json nodesJson;
        nodesFile >> nodesJson;
        nodesFile.close();
        nodes = nodesJson["nodes"].get<std::unordered_map<std::string, std::pair<std::string, unsigned short>>>();
        return true;
    }

    void appendNodeToJson(std::string ip, unsigned short port) {
        std::unordered_map<std::string, std::pair<std::string, unsigned short>> nodes;
        if(getAllNodesFromJson(nodes));
        std::ofstream nodesFile("nodes.json");
        nodes.insert({picosha2::hash256_hex_string(ip+std::to_string(port)), {ip, port}});
        nlohmann::json nodeJson = {{"nodes", nodes}};
        nodesFile << std::setw(2) << nodeJson << std::endl;
        nodesFile.close();
    }
}

#endif