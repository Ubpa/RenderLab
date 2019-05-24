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
			template<typename U>
			const Point operator+(const Vector<1, U> &v) const {
				return Point(x + static_cast<U>(v.x));
			}

			template<typename U>
			Point & operator+=(const Vector<1, U> &v) {
				x += static_cast<U>(v.x);
				return *this;
			}

			template<typename U>
			const Vector<1, T> operator-(const Point<1,U> &p) const {
				return Vector<1, T>(x - static_cast<T>(p.x));
			}

			template<typename U>
			const Point operator-(const Vector<1, U> &v) const {
				return Point(x - static_cast<T>(v.x));
			}

			template<typename U>
			const Point & operator-=(const Vector<1, U> &v) {
				x -= static_cast<T>(v.x);
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_POINT1_H_
