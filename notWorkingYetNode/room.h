#ifndef ROOM_H
#define ROOM_H
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "chat_message.hpp"
#include "peer.h"

class Room {
private:
  std::set<std::shared_ptr<Peer>> _peers;
  enum { max_recent_msgs = 100 };
  std::deque<chat_message> _recent_msgs;
public:
  void join(std::shared_ptr<Peer> peer) {
    _peers.insert(peer);
    for (auto msg: _recent_msgs)
      peer->deliver(msg);
  }

  void leave(std::shared_ptr<Peer> peer) { _peers.erase(peer); }

  void deliver(const chat_message& msg) {
    _recent_msgs.push_back(msg);
    while (_recent_msgs.size() > max_recent_msgs)
      _recent_msgs.pop_front();

    for (auto peer: _peers)
      peer->deliver(msg);
  }
};

#endif
