#ifndef _CPPUTIL_BASIC_MATH_UGM_POINT4_H_
#define _CPPUTIL_BASIC_MATH_UGM_POINT4_H_

#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/Vector4.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val4.h>
#include <CppUtil/Basic/UGM/ext/Metric_Euclidean_4.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Point<4, T> : public EXT::ME_B<4,T,Point<4,T>> {
		public:
			using EXT::ME_B<4, T, Point<4, T>>::ME_B;

		public:
			template<typename U>
			const Point operator+(const Vector<4, U> &v) const {
				return Point(
					x + static_cast<T>(v.x),
					y + static_cast<T>(v.y),
					z + static_cast<T>(v.z),
					w + static_cast<T>(v.w)
				);
			}

			template<typename U>
			Point & operator+=(const Vector<4, U> &v) {
				x += static_cast<T>(v.x);
				y += static_cast<T>(v.y);
				z += static_cast<T>(v.z);
				w += static_cast<T>(v.w);
				return *this;
			}

			template<typename U>
			const Vector<4, T> operator-(const Point<4,U> &p) const {
				return Vector<4, T>(
					x - static_cast<T>(p.x),
					y - static_cast<T>(p.y),
					z - static_cast<T>(p.z),
					w - static_cast<T>(p.w)
				);
			}

			template<typename U>
			const Point operator-(const Vector<4, U> &v) const {
				return Point(
					x - static_cast<T>(v.x),
					y - static_cast<T>(v.y),
					z - static_cast<T>(v.z),
					w - static_cast<T>(v.w)
				);
			}

			template<typename U>
			const Point & operator-=(const Vector<4, U> &v) {
				x -= static_cast<T>(v.x);
				y -= static_cast<T>(v.y);
				z -= static_cast<T>(v.z);
				w -= static_cast<T>(v.w);
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT4_H_
