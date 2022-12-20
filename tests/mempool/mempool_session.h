// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef MEMPOOL_SESSION_H
#define MEMPOOL_SESSION_H
#include <boost/asio.hpp>
#include <unordered_map>
#include <iostream>
#include <deque>
#include "message.hpp"
#include "voter.h"
#include "mempool_room.h"

class MempoolSession : public Voter, public std::enable_shared_from_this<MempoolSession> {
private:
  std::unordered_map<std::string, std::string> &_data;
  boost::asio::ip::tcp::socket _socket;
  MempoolRoom &_room;
  Message _read_msg;

  void _readHeader() {
    auto self(shared_from_this());
      boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.data(), Message::header_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec && _read_msg.decode_header()) _readBody();
        });
  }

  void _readBody() {
    auto self(shared_from_this());
      boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.body(), _read_msg.body_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            std::string message = _read_msg.body();
            if(message.size() > 67) {
              _data.insert({message.substr(0, 34), message.substr(35, 68)});
              for(auto el : _data) std::cout << el.first <<":" << el.second << std::endl;
            }
            _readHeader();
          }
        });
  }

public:
  MempoolSession(boost::asio::ip::tcp::socket socket, MempoolRoom& room, std::unordered_map<std::string, std::string> &data) : _socket(std::move(socket)), _room(room), _data(data) {  std::cout << "MempoolSession C'tor" << std::endl; }

  void start() {
    _room.join(shared_from_this());
    _readHeader();
    }

  ~MempoolSession() { std::cout << "MempoolSession D'tor" << std::endl; }
};

#endif
