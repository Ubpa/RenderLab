#ifndef _CPPUTIL_BASIC_MATH_VECTOR4_H_
#define _CPPUTIL_BASIC_MATH_VECTOR4_H_

#include <CppUtil/Basic/Val4.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector4 : public Val4<T> {
		public:
			using Val4::Val4;

		public:
			float Length2() const { return x * x + y * y + z * z; }
			float Length() const { return std::sqrt(Length2()); }

		public:
			static T Dot(const Vector4<T> & v0, const Vector4<T> & v1) {
				return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + v0.w*v1.w;
			}
			T Dot(const Vector4<T> & v) const {
				return Dot(*this, v);
			}

			const Vector4<T> Norm() const {
				return *this / Length();
			}

			Vector4<T> & NormSelf() {
				(*this) /= Length();
				return *this;
			}

		public:
			const Vector4<T> operator+(const Vector4<T> &v) const {
				return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
			}

			Vector4<T> & operator+=(const Vector4<T> &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				w += v.w;
				return *this;
			}

			const Vector4<T> operator-(const Vector4<T> &v) const {
				return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
			}

			const Vector4<T> operator-() const { return Vector4<T>(-x, -y, -z, -w); }

			Vector4<T> & operator-=(const Vector4<T> &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				w -= v.w;
				return *this;
			}

			template <typename U>
			const Vector4<T> operator*(U s) const {
				return Vector4<T>(s * x, s * y, s * z, s * w);
			}

			template <typename U>
			Vector4<T> & operator*=(U s) {
				x *= s;
				y *= s;
				z *= s;
				w *= s;
				return *this;
			}

			template <typename U>
			const Vector4<T> operator/(U f) const {
				float inv = (float)1 / f;
				return Vector4<T>(x * inv, y * inv, z * inv, w * inv);
			}

			template <typename U>
			Vector4<T> & operator/=(U f) {
				float inv = (float)1 / f;
				x *= inv;
				y *= inv;
				z *= inv;
				w *= inv;
				return *this;
			}
		};

		template <typename U, typename T>
		const Vector4<T> operator * (U a, const Vector4<T> & v) {
			return v * a;
		}

		using Vector4f = Vector4<float>;
		using Vector4i = Vector4<int>;
	}
}

#endif // !_CPPUTIL_BASIC_MATH_VECTOR4_H_
