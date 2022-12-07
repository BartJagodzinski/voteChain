// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

class chat_participant {
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
};

//----------------------------------------------------------------------

class chat_room {
private:
  std::set<std::shared_ptr<chat_participant>> _participants;
  enum { max_recent_msgs = 100 };
  std::deque<chat_message> _recent_msgs;
public:
  void join(std::shared_ptr<chat_participant> participant) {
    _participants.insert(participant);
    for (auto msg: _recent_msgs)
      participant->deliver(msg);
  }

  void leave(std::shared_ptr<chat_participant> participant) { _participants.erase(participant); }

  void deliver(const chat_message& msg) {
    _recent_msgs.push_back(msg);
    while (_recent_msgs.size() > max_recent_msgs)
      _recent_msgs.pop_front();

    for (auto participant: _participants)
      participant->deliver(msg);
  }
};

//----------------------------------------------------------------------

class chat_session : public chat_participant, public std::enable_shared_from_this<chat_session> {
private:
  tcp::socket _socket;
  chat_room& _room;
  chat_message _read_msg;
  std::deque<chat_message> _write_msgs;

  void do_read_header() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.data(), chat_message::header_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec && _read_msg.decode_header()) do_read_body();
          else _room.leave(shared_from_this());
        });
  }

  void do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.body(), _read_msg.body_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            _room.deliver(_read_msg);
            do_read_header();
          }
          else _room.leave(shared_from_this());
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
          else _room.leave(shared_from_this());
        });
  }

public:
  chat_session(tcp::socket socket, chat_room& room) : _socket(std::move(socket)), _room(room) {}

  void start() {
    _room.join(shared_from_this());
    do_read_header();
  }

  void deliver(const chat_message& msg) {
    bool write_in_progress = !_write_msgs.empty();
    _write_msgs.push_back(msg);
    if (!write_in_progress) do_write();
  }
};

//----------------------------------------------------------------------

class chat_server {
private:
  tcp::acceptor _acceptor;
  chat_room _room;

  void do_accept() {
    _acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) std::make_shared<chat_session>(std::move(socket), _room)->start();
          do_accept();
        });
  }

public:
  chat_server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint) : _acceptor(io_context, endpoint) { do_accept(); }
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try {
    if (argc < 2) { std::cerr << "Usage: ./server <port> [<port> ...]\n"; return EXIT_FAILURE; }

    boost::asio::io_context io_context;

    std::list<chat_server> servers;
    for (int i = 1; i < argc; ++i) {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_context, endpoint);
    }

    io_context.run();
  }
  catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

  return EXIT_SUCCESS;
}
