// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef SENDER_H
#define SENDER_H
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include "blockMessage.hpp"

class Sender {
private:
    boost::asio::io_context& _io_context;
    boost::asio::ip::tcp::socket _socket;
    BlockMessage _readBlock;
    std::deque<BlockMessage> _writeBlocks;
    char _buffer[BlockMessage::maxBodyLength];

    void doConnect(const boost::asio::ip::tcp::resolver::results_type& endpoints) { boost::asio::async_connect(_socket, endpoints,
      [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) { if (!ec) doReadHeader(); }); }

    void doReadHeader() {
      boost::asio::async_read(_socket, boost::asio::buffer(_readBlock.data(), BlockMessage::headerLength),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec && _readBlock.decodeHeader()) doReadBody();
          else _socket.close();
        });
    }

    void doReadBody() {
      boost::asio::async_read(_socket, boost::asio::buffer(_readBlock.body(), _readBlock.bodyLength()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            std::ofstream file("receivedBlock.json");
            file.write(_readBlock.body(), _readBlock.bodyLength());
            file.close();
            doReadHeader();
          }
          else _socket.close();
        });
    }

    void doWrite() {
      boost::asio::async_write(_socket, boost::asio::buffer(_writeBlocks.front().data(), _writeBlocks.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            _writeBlocks.pop_front();
            if (!_writeBlocks.empty()) doWrite();
          }
          else _socket.close();
        });
    }
public:
    Sender() = default;
    Sender(boost::asio::io_context& io_context, const boost::asio::ip::tcp::resolver::results_type& endpoints) : _io_context(io_context), _socket(io_context) { doConnect(endpoints); }

    void write(const BlockMessage& block) {
      boost::asio::post(_io_context,
        [this, block]() {
          bool write_in_progress = !_writeBlocks.empty();
          _writeBlocks.push_back(block);
          if (!write_in_progress) doWrite();
        });
    }

    bool saveToBuffer(const std::string &fileName) {
        std::ifstream file(fileName, std::ios::in | std::ios::binary);
        if (!file) return false;        
        file.read(_buffer, BlockMessage::maxBodyLength);
        file.close();
        return true;
    }

    const char * getBuffer() const { return _buffer; }

    void close() { boost::asio::post(_io_context, [this]() { _socket.close(); }); }
};

#endif
