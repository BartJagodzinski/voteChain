// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef CHECKER_SESSION_H
#define CHECKER_SESSION_H
#include <boost/asio.hpp>
#include <iostream>
#include <deque>
#include "voter.h"
#include "checker_room.h"
#include "message.hpp"

class CheckerSession : public Voter, public std::enable_shared_from_this<CheckerSession> {
private:
  boost::asio::ip::tcp::socket _socket;
  CheckerRoom &_room;
  Message _readMsg;
  std::deque<Message> _write_msgs;

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
        if (!ec) { }
        else _room.leave(shared_from_this());
    });
  }

  void _write() {
    auto self(shared_from_this());
    boost::asio::async_write(_socket, boost::asio::buffer(_write_msgs.front().data(), _write_msgs.front().length()),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          _write_msgs.pop_front();
          if (!_write_msgs.empty()) _write();
        }
    });
  }

public:
  CheckerSession(boost::asio::ip::tcp::socket socket, CheckerRoom& room)
  : _socket(std::move(socket)), _room(room) {  std::cout << "CheckerSession C'tor" << std::endl; }

  void start() {
    _room.join(shared_from_this());
    _readHeader();
  }

  void deliver(const Message& msg) {
    bool write_in_progress = !_write_msgs.empty();
    _write_msgs.push_back(msg);
    if (!write_in_progress) _write();
  }

  ~CheckerSession() { std::cout << "CheckerSession D'tor" << std::endl; }
};

#endif
