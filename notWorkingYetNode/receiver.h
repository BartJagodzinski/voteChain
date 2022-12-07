// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef RECEIVER_H
#define RECEIVER_H
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "blockMessage.hpp"

//----------------------------------------------------------------------
class PeerNode {
public:
    virtual ~PeerNode() {}
    virtual void deliver(const BlockMessage& block) = 0;
};
//----------------------------------------------------------------------
class Room {
private:
    std::set<std::shared_ptr<PeerNode>> _peerNodes;
    enum { maxRecentBlocks = 100 };
    std::deque<BlockMessage> _recentBlocks;
public:
    void join(std::shared_ptr<PeerNode> peerNode) {
        _peerNodes.insert(peerNode);
        for (auto block: _recentBlocks)
            peerNode->deliver(block);
    }

    void leave(std::shared_ptr<PeerNode> peerNode) { _peerNodes.erase(peerNode); }

    void deliver(const BlockMessage& block) {
        _recentBlocks.push_back(block);
        while (_recentBlocks.size() > maxRecentBlocks)
            _recentBlocks.pop_front();

        for (auto peerNode: _peerNodes)
            peerNode->deliver(block);
    }
};
//----------------------------------------------------------------------
class Session : public PeerNode, public std::enable_shared_from_this<Session> {
private:
    boost::asio::ip::tcp::socket _socket;
    Room& _room;
    BlockMessage _readBlock;
    std::deque<BlockMessage> _writeBlocks;

    void doReadHeader() {
        auto self(shared_from_this());
        boost::asio::async_read(_socket, boost::asio::buffer(_readBlock.data(), BlockMessage::headerLength),
          [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec && _readBlock.decodeHeader()) doReadBody();
            else _room.leave(shared_from_this());
          });
    }

    void doReadBody() {
        auto self(shared_from_this());
        boost::asio::async_read(_socket, boost::asio::buffer(_readBlock.body(), _readBlock.bodyLength()),
          [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
            _room.deliver(_readBlock);
            doReadHeader();
          }
          else _room.leave(shared_from_this());
        });
  }

    void doWrite() {
        auto self(shared_from_this());
        boost::asio::async_write(_socket, boost::asio::buffer(_writeBlocks.front().data(), _writeBlocks.front().length()),
          [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                _writeBlocks.pop_front();
            if (!_writeBlocks.empty()) doWrite();
            }
            else _room.leave(shared_from_this());
        });
  }

public:
    Session(boost::asio::ip::tcp::socket socket, Room& room) : _socket(std::move(socket)), _room(room) {}

    void start() {
        _room.join(shared_from_this());
        doReadHeader();
    }

    void deliver(const BlockMessage& block) {
        bool write_in_progress = !_writeBlocks.empty();
        _writeBlocks.push_back(block);
        if (!write_in_progress) doWrite();
    }
};
//----------------------------------------------------------------------
class Receiver {
private:
    boost::asio::ip::tcp::acceptor _acceptor;
    Room _room;

    void doAccept() {
        _acceptor.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
                if (!ec) std::make_shared<Session>(std::move(socket), _room)->start();
                doAccept();
            });
  }

public:
    Receiver() = default;
    Receiver(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint) : _acceptor(io_context, endpoint) { doAccept(); }
};

#endif
