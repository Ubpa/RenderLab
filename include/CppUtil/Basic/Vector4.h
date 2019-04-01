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
			static T Dot(const Vector4 & v0, const Vector4 & v1) {
				return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + v0.w*v1.w;
			}
			T Dot(const Vector4 & v) const {
				return Dot(*this, v);
			}

			const Vector4 Norm() const {
				return *this / Length();
			}

			Vector4 & NormSelf() {
				(*this) /= Length();
				return *this;
			}

		public:
			const Vector4 operator+(const Vector4 &v) const {
				return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
			}

			Vector4 & operator+=(const Vector4 &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				w += v.w;
				return *this;
			}

			const Vector4 operator+(T val)const {
				return Vector4(x + val, y + val, z + val, w + val);
			}

			Vector4 operator+=(T val) {
				x += val;
				y += val;
				z += val;
				w += val;
				return *this;
			}

			const Vector4 operator-(const Vector4 &v) const {
				return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
			}

			const Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

			Vector4 & operator-=(const Vector4 &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				w -= v.w;
				return *this;
			}

			const Vector4 operator-(T val)const {
				return Vector4(x - val, y - val, z - val, w - val);
			}

			Vector4 operator-=(T val) {
				x -= val;
				y -= val;
				z -= val;
				w -= val;
				return *this;
			}

			template <typename U>
			const Vector4 operator*(U s) const {
				return Vector4(s * x, s * y, s * z, s * w);
			}

			template <typename U>
			Vector4 & operator*=(U s) {
				x *= s;
				y *= s;
				z *= s;
				w *= s;
				return *this;
			}

			template <typename U>
			const Vector4 operator/(U f) const {
				float inv = (float)1 / f;
				return Vector4(x * inv, y * inv, z * inv, w * inv);
			}

			template <typename U>
			Vector4 & operator/=(U f) {
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

		template<typename T, typename U>
		const Vector4<T> operator+(U a, const Vector4<T> & v) {
			return v + a;
		}

		template<typename T, typename U>
		const Vector4<T> operator-(U a, const Vector4<T> & v) {
			return v - a;
		}
	}

	using Vector4f = Basic::Vector4<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_VECTOR4_H_
