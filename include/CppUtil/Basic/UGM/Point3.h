#ifndef _CPPUTIL_BASIC_MATH_UGM_POINT3_H_
#define _CPPUTIL_BASIC_MATH_UGM_POINT3_H_

#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/Vector3.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val3.h>
#include <CppUtil/Basic/UGM/ext/Metric_Euclidean_3.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Point<3, T> : public EXT::ME_B<3,T,Point<3,T>> {
		public:
			using EXT::ME_B<3, T, Point<3, T>>::ME_B;

		public:
			static float Distance2(const Point & p0, const Point & p1) {
				return (p1 - p0).Norm2();
			}
			float Distance2With(const Point & p) {
				return Distance2(*this, p);
			}

			static float Distance(const Point & p0, const Point & p1) {
				return (p1 - p0).Norm();
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
				return Point(std::min(p0.x, p1.x), std::min(p0.y, p1.y), std::min(p0.z, p1.z));
			}
			template<typename U>
			static Point Min(const Point & p, U val) {
				return Min(p, Point(val));
			}
			const Point MinWith(const Point & p) const{
				return Min(*this, p);
			}
			template<typename U>
			const Point MinWith(U val) const {
				return Min(*this, Point(val));
			}

			static Point Max(const Point & p0, const Point & p1) {
				return Point(std::max(p0.x, p1.x), std::max(p0.y, p1.y), std::max(p0.z, p1.z));
			}
			template<typename U>
			static Point Max(const Point & p, U val) {
				return Max(p, Point(val));
			}
			const Point MaxWith(const Point & p) const {
				return Max(*this, p);
			}
			template<typename U>
			const Point MaxWith(U val) const {
				return Max(*this, Point(val));
			}

		public:
			const Vector<3, T> operator-(const Point &p) const {
				return Vector<3, T>(x - p.x, y - p.y, z - p.z);
			}

			const Point operator+(const Vector<3, T> &v) const {
				return Point(x + v.x, y + v.y, z + v.z);
			}

			Point & operator+=(const Vector<3, T> &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}

			const Point operator-(const Vector<3, T> &v) const {
				return Point(x - v.x, y - v.y, z - v.z);
			}

			const Point & operator-=(const Vector<3, T> &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT3_H_
