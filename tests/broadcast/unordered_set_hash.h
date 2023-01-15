#ifndef UNORDERED_SET_HASH_H
#define UNORDERED_SET_HASH_H
#include <iostream>
#include <tuple>

template <typename T1, typename T2>
struct UnorderedSetTupleHash {
    auto operator()(const std::tuple<T1, T1, T2> &node) const -> size_t {
	   return std::hash<T1>{}(std::get<0>(node)) ^ std::hash<T1>{}(std::get<1>(node)) ^ std::hash<T2>{}(std::get<2>(node));
    }
};

template <typename T1, typename T2>
struct UnorderedSetPairHash {
	auto operator()(const std::pair<T1, T2>& node) const -> size_t {
		return std::hash<T1>{}(node.first) ^ std::hash<T2>{}(node.second);
	}
};

#endif
