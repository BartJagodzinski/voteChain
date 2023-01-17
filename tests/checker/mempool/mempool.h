#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <iostream>
#include <string>
#include <fstream>
#include <boost/asio.hpp>
#include "mempool_session.h"
#include "json.hpp"

class Mempool {
private:
  	boost::asio::io_context& _io_context;
  	boost::asio::ip::tcp::acceptor _acceptor;
  	boost::asio::ip::tcp::socket _socket;
	MempoolRoom _room;
	std::unordered_map<std::string, std::string> _data;
	std::time_t _deadline;

	void _accept() {
    	_acceptor.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
          if (!ec && isOpen()) std::make_shared<MempoolSession>(std::move(socket), _room, _data)->start();
          _accept();
        });
  	}

public:
	Mempool(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint, std::time_t deadline) : _io_context(io_context), _acceptor(io_context, endpoint), _socket(io_context), _deadline(deadline) { _accept(); std::cout << "Mempool C'tor" << std::endl; }

	bool isOpen() { return (std::time(nullptr) < _deadline) ? true : false; }
	bool isEmpty() { return (_data.size() == 0) ? true : false; }
	void close() { boost::asio::post(_io_context, [this]() { _socket.close(); }); }

	void getVotes(std::unordered_map<std::string, std::string> &candidateBlockData, unsigned int nbOfVotes) {
		auto it = _data.begin();
		// Both must be true to avoid removing more votes than are in mempool
		while (it != _data.end() && nbOfVotes > 0) {
			candidateBlockData.insert({it->first, it->second});
			it = _data.erase(it);
			--nbOfVotes;
		}
	}

	bool loadVotesFromJson(std::string fileName) {
		std::ifstream blockJsonFile(fileName);
		if (!blockJsonFile)	return false;
		nlohmann::json blockJson;
		blockJsonFile >> blockJson;
		std::unordered_map<std::string, std::string> votes = blockJson["votes"];
		blockJsonFile.close();
		_data = votes;
		return true;
	}

	~Mempool() { std::cout << "Mempool D'tor" << std::endl; }
};

#endif
