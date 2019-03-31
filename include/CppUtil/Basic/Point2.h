#ifndef _CPPUTIL_BASIC_MATH_POINT2_H_
#define _CPPUTIL_BASIC_MATH_POINT2_H_

#include <CppUtil/Basic/Val2.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector2;

		template <typename T>
		class Point2 : public Val2<T> {
		public:
			using Val2::Val2;

		public:
			static float Distance2(const Point2 & p0, const Point2 & p1) {
				return (p1 - p0).Length2();
			}
			float Distance2With(const Point2 & p) {
				return Distance2(*this, p);
			}

			static float Distance(const Point2 & p0, const Point2 & p1) {
				return (p1 - p0).Length();
			}
			float DistanceWith(const Point2 & p) {
				return Distance(*this, p);
			}

			static Point2 Lerp(const Point2 & p0, const Point2 & p1, float t) {
				return p0 + t * (p1 - p0);
			}
			const Point2 LerpWith(const Point2 p1, float t) {
				return Lerp(*this, p1, t);
			}

			static Point2 Mid(const Point2 & p0, const Point2 & p1) {
				return Lerp(p0, p1, 0.5f);
			}
			static Point2 MidWith(const Point2 p1) {
				return Mid(*this, p1);
			}

			static Point2 Min(const Point2 & p0, const Point2 & p1) {
				return Point2(std::min(p0.x, p1.x), std::min(p0.y, p1.y))
			}

			static Point2 Max(const Point2 & p0, const Point2 & p1) {
				return Point2(std::max(p0.x, p1.x), std::max(p0.y, p1.y));
			}

		public:
			const Vector2<T> operator-(const Point2 &p) const {
				return Vector2<T>(x - p.x, y - p.y);
			}

			const Point2 operator+(const Vector2<T> &v) const {
				return Point2(x + v.x, y + v.y);
			}

			Point2 & operator+=(const Vector2<T> &v) {
				x += v.x;
				y += v.y;
				return *this;
			}

			const Point2 operator-(const Vector2<T> &v) const {
				return Point2(x - v.x, y - v.y);
			}

			const Point2 & operator-=(const Vector2<T> &v) {
				x -= v.x;
				y -= v.y;
				return *this;
			}
		};

		template <typename T>
		std::ostream operator<<(std::ostream & os, const Point2<T> & p) {
			os << "[" << p.x << ", " << p.y << "]";
			return os;
		}
	}

	using Point2f = Basic::Point2<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_POINT2_H_
