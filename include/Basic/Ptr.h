#pragma once

#include <memory>
#include <typeinfo>

namespace Ubpa {
	template<typename T>
	using Ptr = std::shared_ptr<T>;

	template<typename T>
	using PtrC = Ptr<const T>;

	template<typename T>
	using WPtr = std::weak_ptr<T>;

	template<typename T>
	bool operator<(const WPtr<T>& lhs, const WPtr<T>& rhs) {
		return lhs.lock() < rhs.lock();
	}

	template<typename T>
	using WPtrC = WPtr<const T>;

	// use _ to avoid _Vector_alloc, base class of vector
	template<template<typename, typename ...> class ContainerT, typename ValT, typename ... Args>
	ContainerT<PtrC<ValT>> Const(const ContainerT<Ptr<ValT>, Args...>& c) {
		return ContainerT<PtrC<ValT>>(c.begin(), c.end());
	}

	// CastTo
	template<typename FromType, typename ToType, bool isUp, bool isDown>
	struct CastWrapper;
	template<typename FromType, typename ToType>
	struct CastWrapper<FromType, ToType, true, false> {
		static const Ptr<ToType> call(const Ptr<FromType>& ptr) {
			return std::dynamic_pointer_cast<ToType>(ptr);
		}
		static const WPtr<ToType> call(const WPtr<FromType>& ptr) {
			return std::dynamic_pointer_cast<ToType>(ptr.lock());
		}
	};
	template<typename FromType, typename ToType>
	struct CastWrapper<FromType, ToType, false, true> {
		static const Ptr<ToType> call(const Ptr<FromType>& ptr) {
			return std::static_pointer_cast<ToType>(ptr);
		}
		static const WPtr<ToType> call(const WPtr<FromType>& ptr) {
			return std::static_pointer_cast<ToType>(ptr.lock());
		}
	};
	template<typename FromType, typename ToType>
	struct CastWrapper<FromType, ToType, true, true> {
		static const Ptr<ToType> call(const Ptr<FromType>& ptr) {
			return ptr;
		}
		static const WPtr<ToType> call(const WPtr<FromType>& ptr) {
			return ptr;
		}
	};
	/*template<typename FromType, typename ToType>
	struct CastWrapper<FromType, ToType, false, false> {
		static const Ptr<ToType> call(const Ptr<FromType> & ptr) {
			return ERROR_FromType_can_not_cast_to_ToType();
		}
		static const WPtr<ToType> call(const WPtr<FromType> & ptr) {
			return ERROR_FromType_can_not_cast_to_ToType();
		}
	};*/

	template<typename ToType, typename FromType>
	const Ptr<ToType> CastTo(const Ptr<FromType>& ptr) {
		return CastWrapper<FromType, ToType, std::is_base_of_v<FromType, ToType>, std::is_base_of_v<ToType, FromType>>::call(ptr);
	}

	template<typename ToType, typename FromType>
	const WPtr<ToType> CastTo(const WPtr<FromType>& ptr) {
		return CastWrapper<FromType, ToType, std::is_base_of_v<FromType, ToType>, std::is_base_of_v<ToType, FromType>>::call(ptr);
	}
}
