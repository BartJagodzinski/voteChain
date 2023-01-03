#ifndef UNORDERED_SET_HASH_H
#define UNORDERED_SET_HASH_H
#include <iostream>

template <typename T1, typename T2>
struct UnorderedSetHash {
	auto operator()(const std::pair<T1, T2>& vote) const -> size_t {
		return std::hash<T1>{}(vote.first) ^ std::hash<T2>{}(vote.second);
	}
};

#endif
