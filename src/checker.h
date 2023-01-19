// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef CHECKER_H
#define CHECKER_H
#include <iostream>
#include <string>
#include <deque>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <boost/asio.hpp>
#include "checker_session.h"
#include "unordered_set_hash.h"
#include "message.hpp"
#include "picosha2.h"
#include "config.h"

class Checker {
private:
  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::socket _socket;
  boost::asio::ip::tcp::resolver _resolver;
  CheckerRoom _room;
  std::pair<std::string, unsigned short> _ipPort;
  std::deque<Message> _write_msgs;
  Message _read_msg;
  std::vector<char> _buff;

  void _accept() {
    _acceptor.async_accept(
      [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
          std::make_shared<CheckerSession>(std::move(socket), _room)->start();
        }
        _accept();
      });
  }

  void _write() {
    boost::asio::async_write(_socket, boost::asio::buffer(_write_msgs.front().data(), _write_msgs.front().length()),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          _write_msgs.pop_front();
          if (!_write_msgs.empty()) _write();
        }
    });
  }

  void _readHeader() {
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.data(), Message::header_length),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec && _read_msg.decode_header()) _readBody();
      });
  }

  void _readBody() {
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.body(), _read_msg.body_length()),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) { _readHeader(); }
      });
  }

public:
  Checker(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint)
  : _io_context(io_context), _acceptor(io_context, endpoint), _socket(io_context), _resolver(io_context)
  {
    _accept();
    std::cout << "Checker C'tor" << std::endl;
  }

  void write(const Message& msg) {
    boost::asio::post(_io_context,
      [this, msg]() {
        bool write_in_progress = !_write_msgs.empty();
        _write_msgs.push_back(msg);
        if (!write_in_progress) _write();
      });
  }

  void close() { boost::asio::post(_io_context, [this]() { _socket.close(); }); }

  ~Checker() { std::cout << "Checker D'tor" << std::endl;}
};

#endif
