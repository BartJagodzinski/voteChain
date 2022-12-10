#include <iostream>
#include <string>
#include <deque>
#include <unordered_set>
#include <boost/asio.hpp>
#include "session.h"
#include "chat_message.hpp"
#define SIZE 1024

class Node {
private:
  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::acceptor _acceptor;
  Room _room;
  boost::asio::ip::tcp::socket _socket;
  chat_message _read_msg;
  std::deque<chat_message> _write_msgs;

  void do_accept() {
    _acceptor.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
          if (!ec) std::make_shared<Session>(std::move(socket), _room)->start();
          do_accept();
        });
  }

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
  Node(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint) : _io_context(io_context), _acceptor(io_context, endpoint), _socket(io_context) { do_accept(); }

  void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) { boost::asio::async_connect(_socket, endpoints, [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) { if (!ec) do_read_header(); }); }

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
    if (argc < 2) { std::cerr << "Usage: ./node <listenPort> <hostToConnect> <portOfHost>\n"; return EXIT_FAILURE; }

    boost::asio::io_context io_context;
    // Server part of node
    boost::asio::ip::tcp::endpoint listenEndpoint(boost::asio::ip::tcp::v4(), std::atoi(argv[1]));

    Node node(io_context, listenEndpoint);
    boost::asio::ip::tcp::resolver resolver(io_context);
    auto connectEndpoint = resolver.resolve("127.0.0.1", argv[1]);
    node.do_connect(connectEndpoint);

    // Client part of node
    if(argc > 3) {
      auto connectEndpoint = resolver.resolve(argv[2], argv[3]);
      node.do_connect(connectEndpoint);
    }

    std::thread t([&io_context](){ io_context.run(); });

    char line[chat_message::max_body_length + 1];
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {
      chat_message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      node.write(msg);
    }

    node.close();
    t.join();
  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

  return EXIT_SUCCESS;
}
