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
  std::time_t _deadline;
  CheckerRoom _room;
  std::pair<std::string, unsigned short> _ipPort;
  std::deque<Message> _writeMsgs;
  Message _readMsg;
  std::vector<char> _buff;

  void _accept() {
    _acceptor.async_accept(
    [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
      if (!ec) std::make_shared<CheckerSession>(std::move(socket), _room)->start();
      _accept();
    });
  }

  void _write() {
    boost::asio::async_write(_socket, boost::asio::buffer(_writeMsgs.front().data(), _writeMsgs.front().length()),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          _writeMsgs.pop_front();
          if (!_writeMsgs.empty()) _write();
        }
    });
  }

  void _readHeader() {
    boost::asio::async_read(_socket, boost::asio::buffer(_readMsg.data(), Message::header_length),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec && _readMsg.decode_header()) _readBody();
    });
  }

  void _readBody() {
    boost::asio::async_read(_socket, boost::asio::buffer(_readMsg.body(), _readMsg.body_length()),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) { _readHeader(); }
    });
  }

public:
  Checker(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint, std::time_t deadline)
  : _io_context(io_context), _acceptor(io_context, endpoint), _socket(io_context), _resolver(io_context), _deadline(deadline)
  {
    _accept();
    std::cout << "Checker C'tor" << std::endl;
  }

  bool isOpen() { return (std::time(nullptr) < _deadline) ? true : false; }

  void write(const Message& msg) {
    boost::asio::post(_io_context,
      [this, msg]() {
        bool write_in_progress = !_writeMsgs.empty();
        _writeMsgs.push_back(msg);
        if (!write_in_progress) _write();
    });
  }

  void close() { boost::asio::post(_io_context, [this]() { _socket.close(); _io_context.stop(); }); }

  ~Checker() { std::cout << "Checker D'tor" << std::endl;}
};

#endif
