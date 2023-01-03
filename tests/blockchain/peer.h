// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef PEER_H
#define PEER_H
#include "message.hpp"

class Peer {
public:
  virtual ~Peer() {}
  virtual void deliver(const Message& msg) = 0;
};

#endif
