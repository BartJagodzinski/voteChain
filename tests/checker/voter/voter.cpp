// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <unordered_set>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include "message.hpp"
#include "unordered_set_hash.h"
#include "ripemd160.h"
#include "picosha2.h"
#include "convert.h"
#include "json.hpp"
#include "base58.h"
#include "keys.h"
#include "config.h"

class VoteSender {
private:
  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::socket _socket;
  Message _readMsg;
  std::vector<char> _buff;
  std::deque<Message> _writeMsgs;

  void _write() {
    boost::asio::async_write(_socket, boost::asio::buffer(_writeMsgs.front().data(), _writeMsgs.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            _writeMsgs.pop_front();
            if (!_writeMsgs.empty()) _write();
          }
          else _socket.close();
        });
  }

  void _readHeader() {
    boost::asio::async_read(_socket, boost::asio::buffer(_readMsg.data(), Message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec && _readMsg.decode_header()) _readBody();
          else _socket.close();
        });
  }

  void _readBody() {
    boost::asio::async_read(_socket, boost::asio::buffer(_readMsg.body(), _readMsg.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            std::ofstream whitelistFile("whitelist.json", std::ios::app);
            whitelistFile << _readMsg.body();
            whitelistFile.close();
            _readMsg.clear();
            _readHeader();
          }
          else _socket.close();
        });
  }

public:
  VoteSender(boost::asio::io_context& io_context, const boost::asio::ip::tcp::resolver::results_type& endpoints) : _io_context(io_context), _socket(io_context) { connect(endpoints); }

  void connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) { boost::asio::async_connect(_socket, endpoints, [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
    if (!ec) _readHeader();
    else std::cerr << ec.message() << std::endl; });
  }

  void write(const Message& msg) {
    boost::asio::post(_io_context,
        [this, msg]() {
          bool write_in_progress = !_writeMsgs.empty();
          _writeMsgs.push_back(msg);
          if (!write_in_progress) _write();
        });
  }

  void close() { boost::asio::post(_io_context, [this]() { _socket.close(); }); }
};

int main(int argc, char* argv[]) {
  try {
    if (argc < 5) { std::cerr << "Usage: ./voter <email> <password> <name> <surname> <id>" << std::endl; return EXIT_FAILURE; }

    std::string email(argv[1]); std::string password(argv[2]); std::string name(argv[3]); std::string surname(argv[4]); std::string id(argv[5]);
    boost::to_lower(email); boost::to_lower(name); boost::to_lower(surname);
    std::pair<std::string, unsigned short> checkerIpPort;
    if(!config::getEndpointsFromJson(checkerIpPort, email+".json", "checker")) { return EXIT_FAILURE; }

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    auto checkerConnectEndpoint = resolver.resolve(checkerIpPort.first, std::to_string(checkerIpPort.second));
    VoteSender voteSenderToChecker(io_context, checkerConnectEndpoint);
    std::thread thrd([&io_context](){ io_context.run(); });

    bool loop = true;
    std::string fname = "whitelist.json";
    while(loop){
      if (FILE *file = fopen(fname.c_str(), "r")) { fclose(file); loop = false; }
    }

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

    if(!check) {
      std::cerr << "Wrong email / password / name / surname / id, try again." << std::endl;
      std::ofstream overwriteWhitelist("whitelist.json");
      overwriteWhitelist << "";
      overwriteWhitelist.close();
      whitelist.clear();
      return EXIT_FAILURE;
    }
    voteSenderToChecker.close();

    std::pair<std::string, unsigned short> nodeIpPort;
    if(!config::getEndpointsFromJson(nodeIpPort, "node_endpoint.json", "node")) { return EXIT_FAILURE; }


    // If uncompressed key is needed change to keys::pub_key_uncompressed_size
    std::vector<uint8_t> pubKey(keys::pub_key_compressed_size);
    std::vector<uint8_t> secKey(picosha2::k_digest_size);

    keys::privateKeyFromHash(secKey, picosha2::hash256_hex_string(email+password+name+surname+id));

    // If uncompressed key is needed keys::publicKeyFromSecKey(pubKey, secKey, false)
    if(!keys::publicKeyFromSecKey(pubKey, secKey)) return EXIT_FAILURE;
    std::cout << std::endl;
    std::cout << "Secret Key: " << convert::uint8_to_hex_string(secKey.data(), secKey.size()) << std::endl;
    std::cout << "Public Key: " << convert::uint8_to_hex_string(pubKey.data(), pubKey.size()) << std::endl;

    std::vector<uint8_t> pubKeyHash(picosha2::k_digest_size);
    picosha2::hash256(convert::uint8_to_hex_string(pubKey.data(), pubKey.size()), pubKeyHash);

    // If uncompressed key is needed change to keys::pub_key_uncompressed_size
    uint8_t pubKeyHashArr[keys::pub_key_compressed_size];
    uint8_t ripemd160HashArr[ripemd160::k_digest_size];

    // Copy pubKey into array to pass it into ripemd160::hash()
    std::copy(pubKeyHash.begin(), pubKeyHash.end(), pubKeyHashArr);

    ripemd160::hash(pubKeyHashArr, picosha2::k_digest_size, ripemd160HashArr);

    // Copy ripemd160 hash into vector
    std::vector<uint8_t> ripemd160Hash(ripemd160HashArr, ripemd160HashArr + ripemd160::k_digest_size);

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
    std::string address = base58::encode(ripemd160Hash, base58::map);


    auto nodeConnectEndpoint = resolver.resolve(nodeIpPort.first, std::to_string(nodeIpPort.second));
    VoteSender voteSenderToNode(io_context, nodeConnectEndpoint);
    Message msg;
    char vote[65]="";
    std::strcat(vote, address.c_str());
    char receiver[] = ":1JaR2gwbg2vFvgHvshaL61HmCitaCGaBgQ";
    std::strcat(vote, receiver);
    std::cout << vote << std::endl;
    msg.body_length(std::strlen(vote));
    std::memcpy(msg.body(), vote, msg.body_length());
    msg.encode_header();
    voteSenderToNode.write(msg);

    //voteSender.close();
    thrd.join();
  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << std::endl; }

  return EXIT_SUCCESS;
}
