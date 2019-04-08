#ifndef _BASIC_HEADER_PTR_H_
#define _BASIC_HEADER_PTR_H_

#include <memory>
#include <typeinfo>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		using Ptr = std::shared_ptr<T>;

		template<typename T>
		using CPtr = Ptr<const T>;

		template<typename T>
		using WPtr = std::weak_ptr<T>;

		template<typename T>
		bool operator<(const WPtr<T> & lhs, const WPtr<T> & rhs) {
			return lhs.lock() < rhs.lock();
		}

		template<typename T>
		using WCPtr = WPtr<const T>;
	}
}

#endif // !_BASIC_HEADER_PTR_H_
