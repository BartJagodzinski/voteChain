// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef VOTER_H
#define VOTER_H
#include "message.hpp"

class Voter {
public:
  virtual ~Voter() {}
  virtual void deliver(const Message& msg) = 0;
};

#endif
