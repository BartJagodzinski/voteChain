// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef CHECKER_ROOM_H
#define CHECKER_ROOM_H
#include <boost/asio.hpp>
#include <deque>
#include <set>
#include <unordered_set>
#include "message.hpp"
#include "unordered_set_hash.h"
#include "config.h"
#include "voter.h"

class CheckerRoom {
private:
  std::set<std::shared_ptr<Voter>> _voters;
  std::unordered_set<std::tuple<std::string, std::string, std::string, std::string, std::string>, UnorderedSetHashTuple<std::string>> _whitelist;
  std::vector<char> _buff;
  std::vector<Message> _msgs;
public:
  CheckerRoom() {
    if(!config::getWhitelistFromJson(_whitelist)) { exit; }
    loadWhitelistFile();
    std::cout << "CheckerRoom C'tor" << std::endl;
  }

  void join(std::shared_ptr<Voter> voter) {
    _voters.insert(voter);

    for (auto const &msg : _msgs)
      voter->deliver(msg);
    //sendWhitelistToVoter(voter);
  }

  void leave(std::shared_ptr<Voter> voter) { _voters.erase(voter); }

  void loadWhitelistFile() {
    std::ifstream file("whitelist.json", std::ios::in | std::ios::binary);
    // Get file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    // Buffer to store whole file
    _buff.reserve(size);
    _buff.insert(_buff.begin(), std::istream_iterator<char>(file), std::istream_iterator<char>());

    Message msg;
    msg.body_length(Message::max_body_length);

    // Divide buffer to smaller chunks
    char chunk[Message::max_body_length];
    std::deque<char> deq(_buff.begin(), _buff.end());

    // Send chunks until deq.size = 0
    bool loop = true;
    while(loop) {
      memset(chunk, 0, sizeof(chunk));
      for(size_t i = 0; i < 9360; ++i) {
        if(deq.size() == 0) {
          loop = false;
          break;
        }
        chunk[i] = deq[0];
        deq.pop_front();
      }
      std::memcpy(msg.body(), chunk, sizeof(chunk));
      msg.encode_header();
      _msgs.push_back(msg);
    }

    file.close();
  }

  void sendWhitelistToVoter(std::shared_ptr<Voter> voter) {
    Message msg;
    msg.body_length(Message::max_body_length);

    // Divide buffer to smaller chunks
    char chunk[Message::max_body_length];
    std::deque<char> deq(_buff.begin(), _buff.end());

    // Send chunks until deq.size = 0
    bool loop = true;
    while(loop) {
      memset(chunk, 0, sizeof(chunk));
      for(size_t i = 0; i < 9360; ++i) {
        if(deq.size() == 0) {
          loop = false;
          break;
        }
        chunk[i] = deq[0];
        deq.pop_front();
      }
      std::memcpy(msg.body(), chunk, sizeof(chunk));
      msg.encode_header();
      voter->deliver(msg);
    }
  }

  void deliver(const Message& msg) {
    for (auto voter : _voters)
      voter->deliver(msg);
  }

  ~CheckerRoom() { std::cout << "CheckerRoom D'tor" << std::endl; }
};

#endif