#ifndef _CPPUTIL_BASIC_MATH_UGM_POINT1_H_
#define _CPPUTIL_BASIC_MATH_UGM_POINT1_H_

#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/Vector1.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val1.h>
#include <CppUtil/Basic/UGM/ext/Metric_Euclidean_1.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Point<1, T> : public EXT::ME_B<1, T, Point<1, T>> {
		public:
			using EXT::ME_B<1, T, Point<1, T>>::ME_B;

		public:
			const Vector<1, T> operator-(const Point &p) const {
				return Vector<1, T>(x - p.x);
			}

			const Point operator+(const Vector<1, T> &v) const {
				return Point(x + v.x);
			}

			Point & operator+=(const Vector<1, T> &v) {
				x += v.x;
				return *this;
			}

			const Point operator-(const Vector<1, T> &v) const {
				return Point(x - v.x);
			}

			const Point & operator-=(const Vector<1, T> &v) {
				x -= v.x;
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT1_H_
