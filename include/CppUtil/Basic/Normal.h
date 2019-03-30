#ifndef _CPPUTIL_BASIC_MATH_NORMAL_H_
#define _CPPUTIL_BASIC_MATH_NORMAL_H_

#include <CppUtil/Basic/Vector.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Normal : public Vector<T> {
		public:
			using Vector::Vector;
		};

		using Normalf = Normal<float>;
		using Normali = Normal<int>;
	}
}

#endif // !_CPPUTIL_BASIC_MATH_NORMAL_H_
