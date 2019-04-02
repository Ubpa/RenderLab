#ifndef _CPPUTIL_BASIC_MATH_VECTOR_H_
#define _CPPUTIL_BASIC_MATH_VECTOR_H_

namespace CppUtil {
	namespace Basic {
		template<int N, typename T>
		class Vector;

		template<int N, typename T, typename U>
		const Vector<N, T> operator+(U k, const Vector<N, T> & v) {
			return v + k;
		}

		template<int N, typename T, typename U>
		const Vector<N, T> operator-(U k, const Vector<N, T> & v) {
			return -v + k;
		}

		template<int N, typename T, typename U>
		const Vector<N, T> operator*(U k, const Vector<N, T> & v) {
			return v * k;
		}
	}

	using Vec2f = Basic::Vector<2, float>;
	using Vec2 = Vec2f;
	using Vec3f = Basic::Vector<3, float>;
	using Vec3 = Vec3f;
	using Vec4f = Basic::Vector<4, float>;
	using Vec4 = Vec4f;
}

#endif // !_CPPUTIL_BASIC_MATH_VECTOR_H_
