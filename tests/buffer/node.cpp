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
#include "json.hpp"

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
          std::make_shared<Session>(std::move(socket))->start();
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
          //_socket.close();
        });
  }
public:
  Node(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint) : _io_context(io_context), _acceptor(io_context, endpoint), _socket(io_context) { _accept(); std::cout << "Node C'tor" << std::endl; }

  void connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(_socket, endpoints,
      [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) { /*if (!ec) _readHeader();*/ }); }

  void write(const Message& msg) {
    boost::asio::post(_io_context,
      [this, msg]() {
        bool write_in_progress = !_write_msgs.empty();
        _write_msgs.push_back(msg);
        if (!write_in_progress) _write();
      });
  }

  void addMsg(const Message& msg) { _write_msgs.push_back(msg); }

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


    std::thread t([&io_context](){ io_context.run(); });
//*
    // Client part of node
    if(argc > 3) {
      auto connectEndpoint = resolver.resolve(argv[2], argv[3]);
      node.connect(connectEndpoint);

      std::ifstream file("block_1.json", std::ios::in | std::ios::binary);
      // Get file size
      file.seekg(0, std::ios::end);
      int size = file.tellg();
      file.seekg(0, std::ios::beg);
      // Buffer to store whole file
      char buff[size];
      Message msg;
      file.read(buff, size);
      file.close();
      msg.body_length(size);
      std::deque<char> deq(buff, buff + sizeof(buff)/sizeof(*buff));
      memset(buff, 0, sizeof(buff));

      // Divide buffer to smaller chunks
      char chunk[Message::max_body_length];

      // Send header with first vote = 322 B then send 125 votes ( 9374 B) until deq.size = 0
      for(size_t i = 0; i < 322; ++i) {
        chunk[i] = deq[0];
        deq.pop_front();
      }
      std::memcpy(msg.body(), chunk, sizeof(chunk));
      msg.encode_header();
      node.write(msg);

      // Send 125 votes ( 9375 B) until deq.size = 0
      bool loop = true;
      while(loop) {
        memset(chunk, 0, sizeof(chunk));
        for(size_t i = 0; i < 9375; ++i) {
          if(deq.size() == 0) {
            loop = false;
            break;
          }
          chunk[i] = deq[0];
          deq.pop_front();
        }
        std::memcpy(msg.body(), chunk, sizeof(chunk));
        msg.encode_header();
        node.write(msg);
      }
    }

    node.close();
    t.join();
  } catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

  return EXIT_SUCCESS;
}
