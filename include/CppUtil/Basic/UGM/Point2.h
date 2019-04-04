#ifndef _CPPUTIL_BASIC_MATH_UGM_POINT2_H_
#define _CPPUTIL_BASIC_MATH_UGM_POINT2_H_

#include <CppUtil/Basic/UGM/Val2.h>
#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/Vector2.h>

#include <CppUtil/Basic/UGM/ext/Basic_Val2.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Point<2, T> : public EXT::Basic_Val<2, T, Point<2, T>> {
		public:
			using EXT::Basic_Val<2, T, Point<2, T>>::Basic_Val;

		public:
			static float Distance2(const Point & p0, const Point & p1) {
				return (p1 - p0).Length2();
			}
			float Distance2With(const Point & p) {
				return Distance2(*this, p);
			}

			static float Distance(const Point & p0, const Point & p1) {
				return (p1 - p0).Length();
			}
			float DistanceWith(const Point & p) {
				return Distance(*this, p);
			}

		public:
			const Vector<2, T> operator-(const Point &p) const {
				return Vector<2, T>(x - p.x, y - p.y);
			}

			const Point operator+(const Vector<2, T> &v) const {
				return Point(x + v.x, y + v.y);
			}

			Point & operator+=(const Vector<2, T> &v) {
				x += v.x;
				y += v.y;
				return *this;
			}

			const Point operator-(const Vector<2, T> &v) const {
				return Point(x - v.x, y - v.y);
			}

			const Point & operator-=(const Vector<2, T> &v) {
				x -= v.x;
				y -= v.y;
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT2_H_
