#ifndef SESSION_H
#define SESSION_H
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "chat_message.hpp"
#include "peer.h"

class Session : public Peer, public std::enable_shared_from_this<Session> {
private:
  boost::asio::ip::tcp::socket _socket;
  std::shared_ptr<Peer> _peer;
  chat_message _read_msg;
  std::deque<chat_message> _write_msgs;

  void do_read_header() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.data(), chat_message::header_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec && _read_msg.decode_header()) do_read_body();
        });
  }

  void do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.body(), _read_msg.body_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            //_room.deliver(_read_msg);
            std::cout << _read_msg.body() << std::endl;
            do_read_header();
          }
        });
  }

  void do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(_socket, boost::asio::buffer(_write_msgs.front().data(), _write_msgs.front().length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            _write_msgs.pop_front();
            if (!_write_msgs.empty()) do_write();
          }
        });
  }

public:
  Session(boost::asio::ip::tcp::socket socket) : _socket(std::move(socket)) {}

  void start() { do_read_header(); }

  void deliver(const chat_message& msg) {
    bool write_in_progress = !_write_msgs.empty();
    _write_msgs.push_back(msg);
    if (!write_in_progress) do_write();
  }
};

#endif
