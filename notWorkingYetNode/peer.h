#ifndef PEER_H
#define PEER_H
#include "chat_message.hpp"

class Peer {
public:
  virtual ~Peer() {}
  virtual void deliver(const chat_message& msg) = 0;
};

#endif
