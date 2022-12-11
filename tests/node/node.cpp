#include <iostream>
#include <string>
#include <deque>
#include <set>
#include <fstream>
#include <unordered_set>
#include <boost/asio.hpp>
#include "session.h"
#include "unordered_set_hash.h"
#include "message.hpp"
#define SIZE 1024

class Node {
private:
  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::acceptor _acceptor;
  boost::asio::ip::tcp::socket _socket;
  std::unordered_set<std::pair<std::string, unsigned short>, UnorderedSetHash> _peers;
  std::deque<Message> _write_msgs;
  Message _read_msg;

  void _accept() {
    _acceptor.async_accept(
      [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
          _peers.insert({socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port()});
          std::make_shared<Session>(std::move(socket))->start();
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


int main(int argc, char* argv[]) {
  try {
    if (argc < 2) { std::cerr << "Usage: ./node <listenPort> <hostToConnect> <portOfHost>\n"; return EXIT_FAILURE; }

    boost::asio::io_context io_context;
    // Server part of node
    boost::asio::ip::tcp::endpoint listenEndpoint(boost::asio::ip::tcp::v4(), std::atoi(argv[1]));

    Node node(io_context, listenEndpoint);
    boost::asio::ip::tcp::resolver resolver(io_context);

    // Client part of node
    if(argc > 3) {
      auto connectEndpoint = resolver.resolve(argv[2], argv[3]);
      node.do_connect(connectEndpoint);
    }

    std::thread t([&io_context](){ io_context.run(); });

    char line[Message::max_body_length + 1];
    while (std::cin.getline(line, Message::max_body_length + 1)) {
      std::string fileName(line);
      fileName ="block_"+fileName+".json";
      std::ifstream file(fileName, std::ios::in | std::ios::binary);
      Message msg;
      char buff[Message::max_body_length];
      file.read(buff, Message::max_body_length);
      file.close();
      msg.body_length(std::strlen(buff));
      std::memcpy(msg.body(), buff, msg.body_length());
      msg.encode_header();
      node.write(msg);
    }

    node.close();
    t.join();
  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

  return EXIT_SUCCESS;
}
