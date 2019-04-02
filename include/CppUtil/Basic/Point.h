#ifndef _CPPUTIL_BASIC_MATH_POINT_H_
#define _CPPUTIL_BASIC_MATH_POINT_H_

#include <CppUtil/Basic/Val.h>

namespace CppUtil {
	namespace Basic {
		template<int N, typename T>
		class Point;
	}

	using Point2 = Basic::Point<2, float>;
	using Point2i = Basic::Point<2, int>;
	using Point2 = Point2;

	using Point3 = Basic::Point<3, float>;
	using Point3 = Point3;

	using Point4f = Basic::Point<4, float>;
	using Point4 = Point4f;
}

#endif // !_CPPUTIL_BASIC_MATH_POINT_H_
