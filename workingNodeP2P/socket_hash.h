#ifndef SOCKET_HASH_H
#define SOCKET_HASH_H
#include <iostream>

struct SocketHash {
	auto operator()(const std::pair<std::string, unsigned short>& p) const -> size_t {
		return std::hash<std::string>{}(p.first) ^ std::hash<unsigned short>{}(p.second);
	}
};

#endif
