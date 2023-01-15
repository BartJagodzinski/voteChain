// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef NODE_ROOM_H
#define NODE_ROOM_H
#include <boost/asio.hpp>
#include <deque>
#include <set>
#include "peer.h"
#include "message.hpp"

class NodeRoom {
private:
  std::set<std::shared_ptr<Peer>> _peer;
public:
  void join(std::shared_ptr<Peer> peer) { _peer.insert(peer); }

  void leave(std::shared_ptr<Peer> peer) { _peer.erase(peer); }
};

#endif