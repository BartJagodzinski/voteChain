// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef MEMPOOL_ROOM_H
#define MEMPOOL_ROOM_H
#include <boost/asio.hpp>
#include <deque>
#include <set>
#include "voter.h"
#include "message.hpp"

class MempoolRoom {
private:
  std::set<std::shared_ptr<Voter>> _voters;
public:

  void join(std::shared_ptr<Voter> voter) { _voters.insert(voter); }

  void leave(std::shared_ptr<Voter> voter) { _voters.erase(voter); }

  void deliver(const Message& msg) {
    for (auto voter : _voters)
      voter->deliver(msg);
  }
};

#endif
