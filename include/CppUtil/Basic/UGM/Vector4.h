#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_

#include <CppUtil/Basic/UGM/Val4.h>

#include <CppUtil/Basic/UGM/ext/Basic_Val4.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<4, T> : public EXT::Basic_Val<4, T, Vector<4, T>> {
		public:
			using EXT::Basic_Val<4, T, Vector<4, T>>::Basic_Val;

		public:
			T Length2() const { return x * x + y * y + z * z; }
			T Length() const { return std::sqrt(Length2()); }

		public:
			static T Dot(const Vector & v0, const Vector & v1) {
				return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + v0.w*v1.w;
			}
			T Dot(const Vector & v) const {
				return Dot(*this, v);
			}

			const Vector Norm() const {
				assert(Length() != static_cast<T>(0));
				return *this / Length();
			}

			Vector & NormSelf() {
				assert(Length() != static_cast<T>(0));
				(*this) /= Length();
				return *this;
			}

		public:
			template<typename U>
			const Vector operator+(const Vector<4, U> &v) const {
				return Vector(x + v.x, y + v.y, z + v.z, w + v.w);
			}

			template<typename U>
			Vector & operator+=(const Vector<4, U> &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				w += v.w;
				return *this;
			}

			const Vector operator+(T val)const {
				return Vector(x + val, y + val, z + val, w + val);
			}

			Vector operator+=(T val) {
				x += val;
				y += val;
				z += val;
				w += val;
				return *this;
			}

			const Vector operator-() const { return Vector(-x, -y, -z, -w); }

			template<typename U>
			const Vector operator-(const Vector<4, U> &v) const {
				return Vector(x - v.x, y - v.y, z - v.z, w - v.w);
			}

			template<typename U>
			Vector & operator-=(const Vector<4, U> &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				w -= v.w;
				return *this;
			}

			const Vector operator-(T val)const {
				return Vector(x - val, y - val, z - val, w - val);
			}

			Vector operator-=(T val) {
				x -= val;
				y -= val;
				z -= val;
				w -= val;
				return *this;
			}

			template <typename U>
			const Vector operator*(U s) const {
				return Vector(s * x, s * y, s * z, s * w);
			}

			template <typename U>
			Vector & operator*=(U s) {
				x *= s;
				y *= s;
				z *= s;
				w *= s;
				return *this;
			}

			template<typename U>
			const Vector operator*(const Vector<4, U> & rhs) const {
				return Vector(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
			}

			template<typename U>
			Vector & operator*=(const Vector<4, U> & rhs) const {
				x *= rhs.x;
				y *= rhs.y;
				z *= rhs.z;
				w *= rhs.w;
				return *this;
			}

			template <typename U>
			const Vector operator/(U f) const {
				assert(f != static_cast<U>(0));
				float inv = (float)1 / f;
				return Vector(x * inv, y * inv, z * inv, w * inv);
			}

			template <typename U>
			Vector & operator/=(U f) {
				assert(f != static_cast<U>(0));
				float inv = (float)1 / f;
				x *= inv;
				y *= inv;
				z *= inv;
				w *= inv;
				return *this;
			}

			template<typename U>
			const Vector operator/(const Vector<4, U> & rhs) const {
				assert(rhs.x != static_cast<U>(0));
				assert(rhs.y != static_cast<U>(0));
				assert(rhs.z != static_cast<U>(0));
				assert(rhs.w != static_cast<U>(0));
				return Vector(x/rhs.x, y/rhs.y, z/rhs.z, w/rhs.w);
			}

			template<typename U>
			Vector & operator/=(const Vector<4, U> & rhs) const {
				assert(rhs.x != static_cast<U>(0));
				assert(rhs.y != static_cast<U>(0));
				assert(rhs.z != static_cast<U>(0));
				assert(rhs.w != static_cast<U>(0));
				x /= rhs.x;
				y /= rhs.y;
				z /= rhs.z;
				w /= rhs.w;
				return *this;
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_
