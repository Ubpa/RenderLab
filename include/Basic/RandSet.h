#pragma once

#include <random>
#include <vector>

namespace Ubpa {
	template <typename T>
	class RandSet {
	public:
		void Clear();
		T RandPick();
		const T& RandLook();
		void Insert(const T& item);
		size_t Size() const;
		RandSet& operator <<(const T& item);
	protected:
		std::vector<T> data;
	private:
		static std::uniform_int_distribution<unsigned> randMap;
		static std::default_random_engine randEngine;
	};

	template <typename T>
	std::uniform_int_distribution<unsigned> RandSet<T>::randMap;

	template <typename T>
	std::default_random_engine RandSet<T>::randEngine;

	template <typename T>
	void RandSet<T>::Clear() { data.clear(); }

	template <typename T>
	T RandSet<T>::RandPick() {
		size_t idx = randMap(randEngine) % data.size();
		T rst = data[idx];
		data[idx] = data.back();
		data.pop_back();
		return rst;
	}

	template <typename T>
	const T& RandSet<T>::RandLook() {
		size_t idx = randMap(randEngine) % data.size();
		return data[idx];
	}

	template <typename T>
	void RandSet<T>::Insert(const T& item) {
		data.push_back(item);
	}

	template <typename T>
	RandSet<T>& RandSet<T>::operator <<(const T& item) {
		Insert(item);
		return *this;
	}

	template <typename T>
	size_t RandSet<T>::Size() const {
		return data.size();
	}
}
