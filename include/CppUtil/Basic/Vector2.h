#ifndef _CPPUTIL_BASIC_MATH_VECTOR2_H_
#define _CPPUTIL_BASIC_MATH_VECTOR2_H_

#include <CppUtil/Basic/Val2.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Point2;

		template <typename T>
		class Vector2 : public Val2<T> {
		public:
			using Val2::Val2;
			
		public:
			float Length2() const { return x * x + y * y; }
			float Length() const { return std::sqrt(Length2()); }

		public:
			static T Dot(const Vector2<T> & v0, const Vector2<T> & v1) {
				return v0.x*v1.x + v0.y*v1.y;
			}
			T Dot(const Vector2<T> & v) const {
				return Dot(*this, v);
			}

			static T Cross(const Vector2<T> & v0, const Vector2<T> & v1) {
				return Vector2<T>(v0.x*v1.y - v0.y*v1.x);
			}
			T Cross(const Vector2<T> & v) const {
				return Cross(*this, v);
			}

			const Vector2<T> Norm() const {
				return *this / Length();
			}

		public:
			const Vector2<T> operator+(const Vector2<T> &v) const {
				return Vector2(x + v.x, y + v.y);
			}

			const Vector2<T> &operator+=(const Vector2<T> &v) {
				x += v.x;
				y += v.y;
				return *this;
			}

			const Vector2<T> operator-(const Vector2<T> &v) const {
				return Vector2(x - v.x, y - v.y);
			}

			const Vector2<T> operator-() const { return Vector2<T>(-x, -y); }

			const Vector2<T> &operator-=(const Vector2<T> &v) {
				x -= v.x;
				y -= v.y;
				return *this;
			}

			template <typename U>
			const Vector2<T> operator*(U s) const {
				return Vector2<T>(s * x, s * y);
			}

			template <typename U>
			Vector2<T> & operator*=(U s) {
				x *= s;
				y *= s;
				return *this;
			}

			template <typename U>
			const Vector2<T> operator/(U f) const {
				float inv = (float)1 / f;
				return Vector2<T>(x * inv, y * inv);
			}

			template <typename U>
			Vector2<T> & operator/=(U f) {
				float inv = (float)1 / f;
				x *= inv;
				y *= inv;
				return *this;
			}
		};

		template <typename U, typename T>
		const Vector2<T> operator * (U a, const Vector2<T> & v) {
			return v * a;
		}
	}

	using Vector2f = Basic::Vector2<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_VECTOR2_H_
