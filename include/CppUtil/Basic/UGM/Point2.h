#ifndef _CPPUTIL_BASIC_MATH_UGM_POINT2_H_
#define _CPPUTIL_BASIC_MATH_UGM_POINT2_H_

#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/Vector2.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val2.h>
#include <CppUtil/Basic/UGM/ext/Metric_Euclidean_2.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Point<2, T> : public EXT::ME_B<2,T,Point<2,T>> {
		public:
			using EXT::ME_B<2, T, Point<2, T>>::ME_B;

		public:
			template<typename U>
			const Point operator+(const Vector<2, U> &v) const {
				return Point(
					x + static_cast<T>(v.x),
					y + static_cast<T>(v.y)
				);
			}

			template<typename U>
			Point & operator+=(const Vector<2, U> &v) {
				x += static_cast<T>(v.x);
				y += static_cast<T>(v.y);
				return *this;
			}

			template<typename U>
			const Vector<2, T> operator-(const Point<2, U> &p) const {
				return Vector<2, T>(
					x - static_cast<T>(p.x),
					y - static_cast<T>(p.y)
					);
			}

			template<typename U>
			const Point operator-(const Vector<2, U> &v) const {
				return Point(
					x - static_cast<T>(v.x),
					y - static_cast<T>(v.y)
				);
			}

			template<typename U>
			const Point & operator-=(const Vector<2, U> &v) {
				x -= static_cast<T>(v.x);
				y -= static_cast<T>(v.y);
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT2_H_
