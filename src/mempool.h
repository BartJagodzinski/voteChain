#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <iostream>
#include <string>
#include <fstream>
#include <boost/asio.hpp>
#include "mempool_session.h"
#include "json.hpp"
#include "storage.h"


class Mempool {
private:
  	boost::asio::io_context& _io_context;
  	boost::asio::ip::tcp::acceptor _acceptor;
  	boost::asio::ip::tcp::socket _socket;
	std::time_t _deadline;
	MempoolRoom _room;
	std::unordered_map<std::string, std::string> _votesToCheck;
	std::unordered_map<std::string, std::string> _votes;

	void _accept() {
    	_acceptor.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
          if (!ec && isOpen()) std::make_shared<MempoolSession>(std::move(socket), _room, _votesToCheck)->start();
          _accept();
        });
  	}

public:
	Mempool(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint, std::time_t deadline)
	: _io_context(io_context), _acceptor(io_context, endpoint), _socket(io_context), _deadline(deadline)
	{
		_accept();
		std::cout << "Mempool C'tor" << std::endl;
	}

	bool isOpen() { return (std::time(nullptr) < _deadline) ? true : false; }
	bool isEmpty() { return (_votes.size() == 0) ? true : false; }
	bool votesToCheckEmpty() { return (_votesToCheck.size() == 0) ? true : false; }
	void close() { boost::asio::post(_io_context, [this]() { _socket.close(); _io_context.stop(); }); }

	void addLastVote(std::map<std::string, std::string> &candidateBlockVotes, size_t lenght) {
		auto it = _votesToCheck.begin();
		if(storage::checkIfAddressVoted(it->first, lenght) == 0) candidateBlockVotes.insert({it->first, it->second});
	}

	void getVotes(std::map<std::string, std::string> &candidateBlockVotes, unsigned int nbOfVotes, size_t blockchainLenght) {
		if(!storage::checkVotes(_votesToCheck, _votes, blockchainLenght)) std::cerr << "Error in storage::checkVotes" << std::endl;
		auto it = _votes.begin();
		// Both must be true to avoid removing more votes than are in mempool
		while (it != _votes.end() && nbOfVotes > 0) {
			candidateBlockVotes.insert({it->first, it->second});
			it = _votes.erase(it);
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
		_votes = votes;
		return true;
	}

	~Mempool() { std::cout << "Mempool D'tor" << std::endl; }
};

#endif
