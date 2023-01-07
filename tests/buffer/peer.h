#ifndef PEER_H
#define PEER_H
#include "message.hpp"

class Peer {
public:
  virtual ~Peer() {}
  virtual void deliver(const Message& msg) = 0;
};

#endif
