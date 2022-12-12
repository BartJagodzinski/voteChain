// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <string>
#include <deque>
#include <fstream>
#include <unordered_set>
#include <boost/asio.hpp>
#include "node_session.h"
#include "unordered_set_hash.h"
#include "message.hpp"

class Node {
private:
  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::socket _socket;
  std::unordered_set<std::pair<std::string, unsigned short>, UnorderedSetHash<std::string, unsigned short>> _peers;
  std::deque<Message> _write_msgs;
  Message _read_msg;

  void _accept() {
    _acceptor.async_accept(
      [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
          _peers.insert({socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port()});
          std::make_shared<NodeSession>(std::move(socket))->start();
        }
        _accept();
      });
  }

  void _readHeader() {
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.data(), Message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec && _read_msg.decode_header()) _readBody();
          else _socket.close();
        });
  }

  void _readBody() {
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.body(), _read_msg.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            std::cout.write(_read_msg.body(), _read_msg.body_length());
            std::cout << "\n";
            _readHeader();
          }
          else _socket.close();
        });
  }

  void _write() {
    boost::asio::async_write(_socket, boost::asio::buffer(_write_msgs.front().data(), _write_msgs.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            _write_msgs.pop_front();
            if (!_write_msgs.empty()) _write();
          }
          else _socket.close();
        });
  }
public:
  Node(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint) : _io_context(io_context), _acceptor(io_context, endpoint), _socket(io_context) { _accept(); std::cout << "Node C'tor" << std::endl; }

  void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(_socket, endpoints,
      [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) { if (!ec) _readHeader(); }); }

  void write(const Message& msg) {
    boost::asio::post(_io_context,
      [this, msg]() {
        bool write_in_progress = !_write_msgs.empty();
        _write_msgs.push_back(msg);
        if (!write_in_progress) _write();
      });
  }

  void close() { boost::asio::post(_io_context, [this]() { _socket.close(); }); }

  ~Node() { std::cout << "Node D'tor" << std::endl;}
};

#endif
