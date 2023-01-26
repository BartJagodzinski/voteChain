// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VOTE_SENDER_H
#define VOTE_SENDER_H
#include <deque>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include "message.hpp"

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
          std::cout << "Vote sent successfully. " << std::endl;
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
          std::string msg = _readMsg.body();
          std::copy(msg.begin(), msg.end(), std::back_inserter(_buff));
          _readMsg.clear();
          _readHeader();
        }
        else _socket.close();
    });
  }

public:
  VoteSender(boost::asio::io_context& io_context, const boost::asio::ip::tcp::resolver::results_type& endpoints)
  : _io_context(io_context), _socket(io_context) { connect(endpoints); }

  const std::vector<char> &getBuff() { return _buff; }

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

#endif
