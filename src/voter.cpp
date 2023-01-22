// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <unordered_set>
#include <boost/asio.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include "message.hpp"
#include "vote_sender.h"
#include "security.h"
#include "picosha2.h"
#include "address.h"
#include "config.h"

int main(int argc, char* argv[]) {
  try {
    if (argc < 6) { std::cerr << "Usage: ./voter <email> <password> <name> <surname> <id> <receiver>" << std::endl; return EXIT_FAILURE; }

    std::string email(argv[1]); std::string password(argv[2]); std::string name(argv[3]); std::string surname(argv[4]); std::string id(argv[5]); std::string receiver(argv[6]);
    boost::to_lower(email); boost::to_lower(name); boost::to_lower(surname); boost::to_lower(receiver);

    std::pair<std::string, unsigned short> checkerIpPort;
    if(!config::getEndpointsFromJson(checkerIpPort, email+".json", "checker")) { return EXIT_FAILURE; }

    boost::asio::io_context io_context1;
    boost::asio::ip::tcp::resolver resolver1(io_context1);
    auto checkerConnectEndpoint = resolver1.resolve(checkerIpPort.first, std::to_string(checkerIpPort.second));
    VoteSender voteSenderToChecker(io_context1, checkerConnectEndpoint);
    std::thread thrd1([&io_context1](){ io_context1.run(); });

    sleep(1);

    voteSenderToChecker.close();
    thrd1.join();

    std::unordered_set<std::tuple<std::string, std::string, std::string, std::string, std::string>, UnorderedSetHashTuple<std::string>> whitelist;
    if(!config::getWhitelistFromJson(whitelist)) { return EXIT_FAILURE; }
    bool check = false;
    for(auto const &voter : whitelist) {
      if(std::get<0>(voter) == email &&
        std::get<1>(voter) == picosha2::hash256_hex_string(password) &&
        std::get<2>(voter) == picosha2::hash256_hex_string(name) &&
        std::get<3>(voter) == picosha2::hash256_hex_string(surname) &&
        std::get<4>(voter) == picosha2::hash256_hex_string(id))
        {
          check = true;
        }
    }
    security::deleteWhitelistFile();

    if(!check) { std::cerr << "Wrong email / password / name / surname / id, try again." << std::endl; return EXIT_FAILURE; }

    std::pair<std::string, unsigned short> nodeIpPort;
    if(!config::getEndpointsFromJson(nodeIpPort, "node_endpoint.json", "node")) return EXIT_FAILURE;

    std::string address;
    if(!address::getAddress(address, email, password, name, surname, id)) return EXIT_FAILURE;

    std::unordered_set<std::pair<std::string, std::string>, UnorderedSetHashPair<std::string, std::string>> candidates;
    if(!config::getCandidatesFromJson(candidates)) return EXIT_FAILURE;

    std::string voteSting;
    std::string receiverAddr;
    bool isCandidateOnList = false;
    for(auto const &candidate : candidates) {
      if(candidate.first == receiver) {
        voteSting = address + ":" + candidate.second;
        receiverAddr = candidate.second;
        isCandidateOnList = true;
      }
    }

    if(!isCandidateOnList) { std::cerr << "Candidate is not on list, check again your input" << std::endl; return EXIT_FAILURE; }

    char vote[95]="";
    std::strcat(vote, voteSting.c_str());
    std::time_t timestamp = std::time(nullptr);
    std::string timestampStr = ":"+std::to_string(timestamp);
    std::strcat(vote, timestampStr.c_str());

		boost::multiprecision::uint256_t nonce = 0;
		std::string toHash = picosha2::hash256_hex_string(address+receiverAddr+std::to_string(timestamp));
		std::string hash;
		do {
			++nonce;
			hash = toHash + nonce.str();
			hash = picosha2::hash256_hex_string(hash);
    } while (hash.substr(0, 5) != "00000");

    std::string nonceStr = ":"+nonce.str();
    std::strcat(vote, nonceStr.c_str());

    Message msg;
    msg.body_length(std::strlen(vote));
    std::memcpy(msg.body(), vote, msg.body_length());
    msg.encode_header();

    boost::asio::io_context io_context2;
    boost::asio::ip::tcp::resolver resolver2(io_context2);
    auto nodeConnectEndpoint = resolver2.resolve(nodeIpPort.first, std::to_string(nodeIpPort.second));
    VoteSender voteSenderToNode(io_context2, nodeConnectEndpoint);
    std::thread thrd2([&io_context2](){ io_context2.run(); });
    voteSenderToNode.write(msg);
    sleep(1);

    voteSenderToNode.close();
    thrd2.join();
  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << std::endl; }

  return EXIT_SUCCESS;
}
