#ifndef _CPPUTIL_BASIC_MATH_POINT_H_
#define _CPPUTIL_BASIC_MATH_POINT_H_

#include <CppUtil/Basic/Val3.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector;

		template <typename T>
		class Point : public Val3<T> {
		public:
			using Val3::Val3;

		public:
			static float Distance2(const Point<T> & p0, const Point<T> & p1) {
				return (p1 - p0).Length2();
			}
			float Distance2With(const Point<T> & p) {
				return Distance2(*this, p);
			}

			static float Distance(const Point<T> & p0, const Point<T> & p1) {
				return (p1 - p0).Length();
			}
			float DistanceWith(const Point<T> & p) {
				return Distance(*this, p);
			}

			static Point<T> Lerp(const Point<T> & p0, const Point<T> & p1, float t) {
				return p0 + t * (p1 - p0);
			}
			const Point<T> LerpWith(const Point<T> p1, float t) {
				return Lerp(*this, p1, t);
			}

			static Point<T> Mid(const Point<T> & p0, const Point<T> & p1) {
				return Lerp(p0, p1, 0.5f);
			}
			static Point<T> MidWith(const Point<T> p1) {
				return Mid(*this, p1);
			}

			static Point<T> Min(const Point<T> & p0, const Point<T> & p1) {
				return Point<T>(std::min(p0.x, p1.x), std::min(p0.y, p1.y), std::min(p0.z, p1.z));
			}
			const Point<T> MinWith(const Point<T> & p) const{
				return Point<T>(std::min(x, p.x), std::min(y, p.y), std::min(z, p.z));
			}

			static Point<T> Max(const Point<T> & p0, const Point<T> & p1) {
				return Point<T>(std::max(p0.x, p1.x), std::max(p0.y, p1.y), std::max(p0.z, p1.z));
			}
			const Point<T> MaxWith(const Point<T> & p) const {
				return Point<T>(std::max(x, p.x), std::max(y, p.y), std::max(z, p.z));
			}

		public:
			const Vector<T> operator-(const Point<T> &p) const {
				return Vector<T>(x - p.x, y - p.y, z - p.z);
			}

			const Point<T> operator+(const Vector<T> &v) const {
				return Point(x + v.x, y + v.y, z + v.z);
			}

			Point<T> & operator+=(const Vector<T> &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}

			const Point<T> operator-(const Vector<T> &v) const {
				return Point<T>(x - v.x, y - v.y, z - v.z);
			}

			const Point<T> & operator-=(const Vector<T> &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return *this;
			}
		};
	}

	using Pointf = Basic::Point<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_POINT_H_
