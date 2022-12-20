// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

class chat_client {
private:
  boost::asio::io_context& _io_context;
  tcp::socket _socket;
  chat_message _read_msg;
  std::deque<chat_message> _write_msgs;

  void do_connect(const tcp::resolver::results_type& endpoints) { boost::asio::async_connect(_socket, endpoints, [this](boost::system::error_code ec, tcp::endpoint) { if (!ec) do_read_header(); }); }

  void do_read_header() {
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.data(), chat_message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec && _read_msg.decode_header()) do_read_body();
          else _socket.close();
        });
  }

  void do_read_body() {
    boost::asio::async_read(_socket, boost::asio::buffer(_read_msg.body(), _read_msg.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            std::cout.write(_read_msg.body(), _read_msg.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else _socket.close();
        });
  }

  void do_write() {
    boost::asio::async_write(_socket, boost::asio::buffer(_write_msgs.front().data(), _write_msgs.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            _write_msgs.pop_front();
            if (!_write_msgs.empty()) do_write();
          }
          else _socket.close();
        });
  }
public:
  chat_client(boost::asio::io_context& io_context, const tcp::resolver::results_type& endpoints) : _io_context(io_context), _socket(io_context) { do_connect(endpoints); }

  void write(const chat_message& msg) {
    boost::asio::post(_io_context,
        [this, msg]() {
          bool write_in_progress = !_write_msgs.empty();
          _write_msgs.push_back(msg);
          if (!write_in_progress) do_write();
        });
  }

  void close() { boost::asio::post(_io_context, [this]() { _socket.close(); }); }
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 3) { std::cerr << "Usage: ./client <host> <port>\n"; return EXIT_FAILURE; }

    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    chat_client chat(io_context, endpoints);
    std::thread thrd([&io_context](){ io_context.run(); });

    char line[2];
    char sender[] = "15e15hWo6CShMgbAfo8c2Ykj4C6BLq6No";
    char receiver[] = ":1JaR2gwbg2vFvgHvshaL61HmCitaCGaBgQ";
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {
      std::strcat(sender, line);
      std::cout << sender << std::endl;
      chat_message msg;
      char vote[65]="";
      std::strcat(vote, sender);
      std::strcat(vote, receiver);
      std::cout << vote << std::endl;
      msg.body_length(std::strlen(vote));
      std::memcpy(msg.body(), vote, msg.body_length());
      msg.encode_header();
      chat.write(msg);
    }

    chat.close();
    thrd.join();
  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

  return EXIT_SUCCESS;
}
