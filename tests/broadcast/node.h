// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <string>
#include <deque>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <boost/asio.hpp>
#include "node_session.h"
#include "unordered_set_hash.h"
#include "message.hpp"
#include "picosha2.h"
#include "node_config.h"

class Node {
private:
  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::socket _socket;
  boost::asio::ip::tcp::resolver _resolver;
	NodeRoom _room;
  std::pair<std::string, unsigned short> _ipPort;
  std::unordered_map<std::string, std::pair<std::string, unsigned short>> _peers;
  std::deque<Message> _write_msgs;
  Message _read_msg;
  std::vector<char> _buff;

  void _accept() {
    _acceptor.async_accept(
      [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
          std::make_shared<NodeSession>(std::move(socket), _room)->start();
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

public:
  Node(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint)
  : _io_context(io_context), _acceptor(io_context, endpoint), _socket(io_context), _resolver(io_context)
  {
    _ipPort = config::getThisNodeIpPort();
    if(!config::getAllNodesFromJson(_peers)) { std::cerr << "Error in reading nodes from json" << std::endl; exit;}
    _accept(); std::cout << "Node C'tor" << std::endl;
  }

  void connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(_socket, endpoints,
      [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) { /*if (!ec) _readHeader();*/ });
  }

  void write(const Message& msg) {
    boost::asio::post(_io_context,
      [this, msg]() {
        bool write_in_progress = !_write_msgs.empty();
        _write_msgs.push_back(msg);
        if (!write_in_progress) _write();
      });
  }

  void loadBlockFileToBuffer(unsigned int id) {
    std::ifstream file("block_"+std::to_string(id)+".json", std::ios::in | std::ios::binary);
    // Get file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    // Buffer to store whole file
    _buff.reserve(size);
    _buff.insert(_buff.begin(), std::istream_iterator<char>(file), std::istream_iterator<char>());
    file.close();
  }

  void sendBlockToPeer() {
    Message msg;
    msg.body_length(Message::max_body_length);

    // Divide buffer to smaller chunks
    char chunk[Message::max_body_length];
    std::deque<char> deq(_buff.begin(), _buff.end());

    // Send chunks until deq.size = 0
    bool loop = true;
    while(loop) {
      memset(chunk, 0, sizeof(chunk));
      for(size_t i = 0; i < 9360; ++i) {
        if(deq.size() == 0) {
          loop = false;
          break;
        }
        chunk[i] = deq[0];
        deq.pop_front();
      }
      std::memcpy(msg.body(), chunk, sizeof(chunk));
      msg.encode_header();
      write(msg);
    }
  }

  void broadcastBlock(unsigned int id) {
    _buff.clear();
    loadBlockFileToBuffer(id);
    for(auto peer : _peers) {
      // If peer == this node -> skip iteration
      if(peer.second.first == _ipPort.first && peer.second.second == _ipPort.second) continue;
      std::cout << "Sending to: " << peer.second.first << ":" << std::to_string(peer.second.second) << std::endl;
      auto connectEndpoint = _resolver.resolve(peer.second.first, std::to_string(peer.second.second));
      connect(connectEndpoint);
      sendBlockToPeer();
    }
    _buff.clear();
  }

  void close() { boost::asio::post(_io_context, [this]() { _socket.close(); }); }

  ~Node() { std::cout << "Node D'tor" << std::endl;}
};

#endif
