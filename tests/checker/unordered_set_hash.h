#ifndef UNORDERED_SET_HASH_H
#define UNORDERED_SET_HASH_H
#include <iostream>
#include <tuple>

template <typename T1>
struct UnorderedSetHashTuple {
	auto operator()(const std::tuple<T1, T1, T1, T1, T1>& voter) const -> size_t {
		return std::hash<T1>{}(std::get<0>(voter)) ^ std::hash<T1>{}(std::get<1>(voter)) ^ std::hash<T1>{}(std::get<2>(voter)) ^ std::hash<T1>{}(std::get<3>(voter)) ^ std::hash<T1>{}(std::get<4>(voter));
	}
};

#endif
