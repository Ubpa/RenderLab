#ifndef _CPPUTIL_BASIC_MATH_VAL_H_
#define _CPPUTIL_BASIC_MATH_VAL_H_

#include <CppUtil/Basic/Math.h>

#include <cassert>
#include <iostream>

namespace CppUtil {
	namespace Basic {
		template<int N, typename T>
		class Val;

		template <int N, typename T>
		std::ostream & operator<<(std::ostream & os, const Val<N, T> & valN) {
			os << "[";
			for (int i = 0; i < N - 1; i++)
				os << Math::ToZero(valN[i]) << ", ";
			os << Math::ToZero(valN[N - 1]) << "]";
			return os;
		}
	}

	template <int N, typename T>
	using Val = Basic::Val<N, T>;

	using Val2f = Val<2, float>;
	using Val2i = Val<2, int>;
	using Val2 = Val2f;

	using Val3f = Val<3, float>;
	using Val3i = Val<3, int>;
	using Val3 = Val3f;

	using Val4f = Val<4, float>;
	using Val4i = Val<4, int>;
	using Val4 = Val4f;
}

#endif // !_CPPUTIL_BASIC_MATH_VAL_H_
