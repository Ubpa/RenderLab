#ifndef _CPPUTIL_BASIC_MATH_UGM_POINT_H_
#define _CPPUTIL_BASIC_MATH_UGM_POINT_H_

#include <Basic/UGM/Val.h>

namespace CppUtil {
	namespace Basic {
		template<int N, typename T>
		class Point;
	}

	template <int N, typename T>
	using Point = Basic::Point<N, T>;

	using Point1f = Point<1, float>;
	using Point1 = Point1f;

	using Point2f = Point<2, float>;
	using Point2i = Point<2, int>;
	using Point2ui = Point<2, unsigned int>;
	using Point2 = Point2f;

	using Point3 = Point<3, float>;
	using Point3 = Point3;

	using Point4f = Point<4, float>;
	using Point4 = Point4f;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT_H_
