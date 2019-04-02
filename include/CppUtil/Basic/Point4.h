#ifndef _CPPUTIL_BASIC_MATH_POINT4_H_
#define _CPPUTIL_BASIC_MATH_POINT4_H_

#include <CppUtil/Basic/Val4.h>
#include <CppUtil/Basic/Point.h>
#include <CppUtil/Basic/Vector4.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Point<4, T> : public Val<4, T> {
		public:
			using Val<4, T>::Val;

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

			static Point Lerp(const Point & p0, const Point & p1, float t) {
				return p0 + t * (p1 - p0);
			}
			const Point LerpWith(const Point p1, float t) {
				return Lerp(*this, p1, t);
			}

			static Point Mid(const Point & p0, const Point & p1) {
				return Lerp(p0, p1, 0.5f);
			}
			static Point MidWith(const Point p1) {
				return Mid(*this, p1);
			}

			static Point Min(const Point & p0, const Point & p1) {
				return Point(std::min(p0.x, p1.x), std::min(p0.y, p1.y), std::min(p0.z, p1.z), std::min(p0.w, p1.w));
			}
			const Point MinWith(const Point & p) const {
				return Min(*this, p);
			}

			static Point Max(const Point & p0, const Point & p1) {
				return Point(std::max(p0.x, p1.x), std::max(p0.y, p1.y), std::max(p0.z, p1.z), std::max(p0.w, p1.w));
			}
			const Point MaxWith(const Point & p) const {
				return Max(*this, p);
			}

		public:
			const Vector<4, T> operator-(const Point &p) const {
				return Vector<4, T>(x - p.x, y - p.y, z - p.z, w - p.w);
			}

			const Point operator+(const Vector<4, T> &v) const {
				return Point(x + v.x, y + v.y, z + v.z, w + v.w);
			}

			Point & operator+=(const Vector<4, T> &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				w += v.w;
				return *this;
			}

			const Point operator-(const Vector<4, T> &v) const {
				return Point(x - v.x, y - v.y, z - v.z, w - v.w);
			}

			const Point & operator-=(const Vector<4, T> &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				w -= v.w;
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_POINT4_H_
