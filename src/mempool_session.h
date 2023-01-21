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

class MempoolSession : public Voter, public std::enable_shared_from_this<MempoolSession> {
private:
  std::unordered_map<std::string, std::string> &_votesToCheck;
  boost::asio::ip::tcp::socket _socket;
  MempoolRoom &_room;
  Message _readMsg;

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
          if(message.size() > 67 && message.at(0) == '1' && message.at(34) == ':' && message.at(35) == '1') _votesToCheck.insert({message.substr(0, 34), message.substr(35, 68)});
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
