#ifndef _CPPUTIL_BASIC_MATH_VECTOR2_H_
#define _CPPUTIL_BASIC_MATH_VECTOR2_H_

#include <CppUtil/Basic/Val2.h>
#include <CppUtil/Basic/Vector.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<2, T> : public Val<2, T> {
		public:
			using Val<2, T>::Val;
			
		public:
			T Length2() const { return x * x + y * y; }
			T Length() const { return std::sqrt(Length2()); }

		public:
			static T Dot(const Vector & v0, const Vector & v1) {
				return v0.x*v1.x + v0.y*v1.y;
			}
			T Dot(const Vector & v) const {
				return Dot(*this, v);
			}

			static T Cross(const Vector & v0, const Vector & v1) {
				return Vector(v0.x*v1.y - v0.y*v1.x);
			}
			T Cross(const Vector & v) const {
				return Cross(*this, v);
			}

			const Vector Norm() const {
				return *this / Length();
			}

		public:
			const Vector operator+(const Vector &v) const {
				return Vector(x + v.x, y + v.y);
			}

			const Vector &operator+=(const Vector &v) {
				x += v.x;
				y += v.y;
				return *this;
			}

			const Vector operator+(T val)const {
				return Vector(x + val, y + val);
			}

			Vector operator+=(T val) {
				x += val;
				y += val;
				return *this;
			}

			const Vector operator-() const { return Vector(-x, -y); }

			const Vector operator-(const Vector &v) const {
				return Vector(x - v.x, y - v.y);
			}

			const Vector &operator-=(const Vector &v) {
				x -= v.x;
				y -= v.y;
				return *this;
			}

			const Vector operator-(T val)const {
				return Vector(x - val, y - val);
			}

			Vector operator-=(T val) {
				x -= val;
				y -= val;
				return *this;
			}

			template <typename U>
			const Vector operator*(U s) const {
				return Vector(s * x, s * y);
			}

			template <typename U>
			Vector & operator*=(U s) {
				x *= s;
				y *= s;
				return *this;
			}

			template <typename U>
			const Vector operator/(U f) const {
				float inv = (float)1 / f;
				return Vector(x * inv, y * inv);
			}

			template <typename U>
			Vector & operator/=(U f) {
				float inv = (float)1 / f;
				x *= inv;
				y *= inv;
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_VECTOR2_H_
