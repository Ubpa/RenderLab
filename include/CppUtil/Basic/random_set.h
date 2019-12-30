#pragma once
#ifndef _CPPUTIL_BASIC_HEADER_RANDOM_SET_H_
#define _CPPUTIL_BASIC_HEADER_RANDOM_SET_H_

#include <unordered_map>
#include <vector>
#include <stack>

template<typename T, class Alloc = std::allocator<T>>
class random_set {
public:
	void insert(const T & e) {
		Tmap[e] = Tvec.size();
		Tvec.push_back(e);
	}

	void erase(const T & e) {
		auto target = Tmap.find(e);
		if (target == Tmap.end())
			return;
		auto eIdx = target->second;
		if (eIdx != Tvec.size() - 1 && Tvec.size() > 1) {
			Tmap[Tvec.back()] = eIdx;
			Tvec[eIdx] = Tvec.back();
		}
		Tvec.pop_back();
		Tmap.erase(target);
	}

	auto begin() { return Tvec.begin(); }
	auto begin() const { return Tvec.begin(); }
	auto end() { return Tvec.end(); }
	auto end() const { return Tvec.end(); }

	T & operator[](size_t i) { return Tvec[i]; }
	const T & operator[](size_t i) const { return Tvec[i]; }

	size_t size() const { return Tvec.size(); }
	void reserve(size_t n) {
		Tvec.reserve(n);
		Tmap.reserve(n);
	}
	void clear() {
		Tvec.clear();
		Tmap.clear();
	}

	const std::vector<T> & vec() const { return Tvec; }

	size_t idx(const T & e) const {
		auto target = Tmap.find(e);
		if (target == Tmap.end())
			return -1;
		return target->second;
	}
private:
	std::unordered_map<T, size_t> Tmap;
	std::vector<T, Alloc> Tvec;
};

#endif // !_CPPUTIL_BASIC_HEADER_RANDOM_SET_H_
