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
			template<typename U>
			const Point operator+(const Vector<3, U> &v) const {
				return Point(
					x + static_cast<T>(v.x),
					y + static_cast<T>(v.y),
					z + static_cast<T>(v.z)
				);
			}

			template<typename U>
			Point & operator+=(const Vector<3, U> &v) {
				x += static_cast<T>(v.x);
				y += static_cast<T>(v.y);
				z += static_cast<T>(v.z);
				return *this;
			}

			template<typename U>
			const Vector<3, T> operator-(const Point<3, U> &p) const {
				return Vector<3, T>(
					x - static_cast<T>(p.x),
					y - static_cast<T>(p.y),
					z - static_cast<T>(p.z)
				);
			}

			template<typename U>
			const Point operator-(const Vector<3, U> &v) const {
				return Point(
					x - static_cast<T>(v.x),
					y - static_cast<T>(v.y),
					z - static_cast<T>(v.z)
				);
			}

			template<typename U>
			const Point & operator-=(const Vector<3, U> &v) {
				x -= static_cast<T>(v.x);
				y -= static_cast<T>(v.y);
				z -= static_cast<T>(v.z);
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT3_H_
