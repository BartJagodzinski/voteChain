#ifndef MEMPOOL_ROOM_H
#define MEMPOOL_ROOM_H
#include <boost/asio.hpp>
#include <deque>
#include <set>
#include "voter.h"
#include "message.hpp"

class MempoolRoom {
private:
  std::set<std::shared_ptr<Voter>> _voter;
  enum { max_recent_msgs = 100 };
  std::deque<Message> _recent_msgs;
public:
  void join(std::shared_ptr<Voter> voter) { _voter.insert(voter); }

  void leave(std::shared_ptr<Voter> voter) { _voter.erase(voter); }
};

#endif
