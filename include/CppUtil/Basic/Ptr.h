#ifndef _PTR_H_
#define _PTR_H_

#include <memory>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		using Ptr = std::shared_ptr<T>;

		template<typename T>
		using CPtr = std::shared_ptr<const T>;
	}
}

#endif // !_PTR_H_
