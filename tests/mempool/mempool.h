#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "mempool_session.h"

class Mempool {
private:
  	boost::asio::ip::tcp::acceptor _acceptor;
	MempoolRoom _room;
	std::unordered_map<std::string, std::string> _data;
	std::time_t _deadline;

	bool _isOpen() { return (std::time(nullptr) < _deadline) ? true : false; }

	void _accept() {
    	_acceptor.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
          if (!ec && _isOpen()) std::make_shared<MempoolSession>(std::move(socket), _room, _data)->start();
          _accept();
        });
  	}

public:
	Mempool(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint, std::time_t deadline) : _acceptor(io_context, endpoint), _deadline(deadline) { _accept(); std::cout << "Mempool C'tor" << std::endl; }

	void getVotes(std::unordered_map<std::string, std::string> &candidateBlockData, unsigned int nbOfVotes) {
		auto it = _data.begin();
		// Both must be true to avoid removing more votes than are in mempool
		while (it != _data.end() && nbOfVotes > 0) {
			candidateBlockData.insert({it->first, it->second});
			it = _data.erase(it);
			--nbOfVotes;
		}
	}

	~Mempool() { std::cout << "Mempool D'tor" << std::endl; }
};

#endif
