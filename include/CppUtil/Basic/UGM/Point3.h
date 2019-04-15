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
			const Point operator+(const Vector<3, T> &v) const {
				return Point(x + v.x, y + v.y, z + v.z);
			}

			Point & operator+=(const Vector<3, T> &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}

			const Vector<3, T> operator-(const Point &p) const {
				return Vector<3, T>(x - p.x, y - p.y, z - p.z);
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

#ifndef NDEBUG
			// 仅用于 Debug 时方便在 IDE 中显示结果
		public:
			Point & operator =(const Point & p) {
				*static_cast<EXT::ME_B<3, T, Point<3, T>>*>(this) = p;
				return *this;
			}
		private:
			const T & _x{ *(((T*)this) + 0) };
			const T & _y{ *(((T*)this) + 1) };
			const T & _z{ *(((T*)this) + 2) };
#endif // !NDEBUG
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT3_H_
