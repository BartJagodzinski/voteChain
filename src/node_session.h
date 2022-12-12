// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef NODE_SESSION_H
#define NODE_SESSION_H
#include <boost/asio.hpp>
#include <iostream>
#include <deque>
#include "peer.h"
#include "message.hpp"

class NodeSession : public Peer, public std::enable_shared_from_this<NodeSession> {
private:
  boost::asio::ip::tcp::socket _socket;
  std::shared_ptr<Peer> _peer;
  Message _read_msg;
  std::deque<Message> _write_msgs;

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
            std::cout << _read_msg.body() << std::endl;
            _readHeader();
          }
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
  NodeSession(boost::asio::ip::tcp::socket socket) : _socket(std::move(socket)) {  std::cout << "NodeSession C'tor" << std::endl; }

  void start() { _readHeader(); }

  void deliver(const Message& msg) {
    bool write_in_progress = !_write_msgs.empty();
    _write_msgs.push_back(msg);
    if (!write_in_progress) _write();
  }

  ~NodeSession() { std::cout << "NodeSession D'tor" << std::endl; }
};

#endif
