#pragma once

#include <map>

namespace Ubpa {
	// Local Storage
	template<typename ID_Type, typename T>
	class LStorage {
	public:
		// 第一次注册返回 true, 后续返回 false
		bool Reg(const ID_Type& uniqueID, const T& item);

		bool UnReg(const ID_Type& uniqueID);

		T* GetP(const ID_Type& uniqueID);

		const T* GetP(const ID_Type& uniqueID) const;

		bool GetV(const ID_Type& uniqueID, T& item);

	private:
		std::map<ID_Type, T> directory;
	};

	//----------------------------------------------------------------------

	template<typename ID_Type, typename T>
	bool LStorage<ID_Type, T>::Reg(const ID_Type& uniqueID, const T& item) {
		auto target = directory.find(uniqueID);
		if (target != directory.end()) {
			target->second = item;
			return false;
		}

		directory[uniqueID] = item;
		return true;
	}

	template<typename ID_Type, typename T>
	bool LStorage<ID_Type, T>::UnReg(const ID_Type& uniqueID) {
		auto target = directory.find(uniqueID);
		if (target == directory.end())
			return false;

		directory.erase(target);
		return true;
	}

	template<typename ID_Type, typename T>
	T* LStorage<ID_Type, T>::GetP(const ID_Type& uniqueID) {
		auto target = directory.find(uniqueID);
		if (target == directory.end())
			return NULL;

		return &(target->second);
	}

	template<typename ID_Type, typename T>
	const T* LStorage<ID_Type, T>::GetP(const ID_Type& uniqueID) const {
		auto target = directory.find(uniqueID);
		if (target == directory.end())
			return NULL;

		return &(target->second);
	}

	template<typename ID_Type, typename T>
	bool LStorage<ID_Type, T>::GetV(const ID_Type& uniqueID, T& item) {
		auto target = directory.find(uniqueID);
		if (target == directory.end())
			return false;

		item = target->second;
		return true;
	}
}
