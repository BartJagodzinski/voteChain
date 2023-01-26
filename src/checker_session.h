// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef MEMPOOL_SESSION_H
#define MEMPOOL_SESSION_H
#include <boost/asio.hpp>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <deque>
#include "mempool_room.h"
#include "message.hpp"
#include "voter.h"
#include "json.hpp"
#include "picosha2.h"

class MempoolSession : public Voter, public std::enable_shared_from_this<MempoolSession> {
private:
  std::unordered_map<std::string, std::string> &_votesToCheck;
  boost::asio::ip::tcp::socket _socket;
  MempoolRoom &_room;
  Message _readMsg;
	unsigned short _difficulty = 5;
	const std::string _target = "00000000000000000000";

  void _readHeader() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket, boost::asio::buffer(_readMsg.data(), Message::header_length),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec && _readMsg.decode_header()) _readBody();
        else _room.leave(shared_from_this());
    });
  }

  void _readBody() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket, boost::asio::buffer(_readMsg.body(), _readMsg.body_length()),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          std::string message = _readMsg.body();
          if(message.size() > 81) {
            std::string sender = message.substr(0, 34);
            std::string receiver = message.substr(35, 34);
            std::string timestamp = message.substr(70, 10);
            std::string nonce = message.substr(81, message.size()-70);
            std::string::size_type pos = nonce.find(';');
            if (pos != std::string::npos) nonce = nonce.substr(0, pos);
            std::string hash = picosha2::hash256_hex_string(sender+receiver+timestamp+nonce);
            if(sender.at(0) == '1' && receiver.at(0) == '1' && hash.substr(0, _difficulty) == _target.substr(0, _difficulty))
              _votesToCheck.insert({message.substr(0, 34), message.substr(35, 34)});
          }
        }
        _room.leave(shared_from_this());
    });
  }

public:
  MempoolSession(boost::asio::ip::tcp::socket socket, MempoolRoom &room, std::unordered_map<std::string, std::string> &data)
  : _socket(std::move(socket)), _room(room), _votesToCheck(data) { std::cout << "MempoolSession C'tor" << std::endl; }

  void start() {
    _room.join(shared_from_this());
    _readHeader();
  }

  void deliver(const Message& msg) {}

  ~MempoolSession() { std::cout << "MempoolSession D'tor" << std::endl; }
};

#endif
