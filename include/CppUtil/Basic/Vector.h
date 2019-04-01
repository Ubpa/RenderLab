#ifndef _CPPUTIL_BASIC_MATH_VECTOR_H_
#define _CPPUTIL_BASIC_MATH_VECTOR_H_

#include <CppUtil/Basic/Val3.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Point;
		template <typename T>
		class Normal;

		template <typename T>
		class Vector : public Val3<T> {
		public:
			using Val3::Val3;
			
		public:
			float Length2() const { return x * x + y * y + z * z; }
			float Length() const { return std::sqrt(Length2()); }

		public:
			static T Dot(const Vector & v0, const Vector & v1) {
				return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z;
			}
			T Dot(const Vector & v) const {
				return Dot(*this, v);
			}

			static const Vector Cross(const Vector & v0, const Vector & v1) {
				//  i,  j,  k
				// x0, y0, z0
				// x1, y1, z1
				return Vector(v0.y*v1.z - v0.z*v1.y, v0.z*v1.x - v0.x*v1.z, v0.x*v1.y - v0.y*v1.x);
			}
			const Vector Cross(const Vector & v) const {
				return Cross(*this, v);
			}

			const Vector Norm() const {
				return *this / Length();
			}

			Vector & NormSelf() {
				(*this) /= Length();
				return *this;
			}

		public:
			const Vector operator+(const Vector &v) const {
				return Vector(x + v.x, y + v.y, z + v.z);
			}

			Vector & operator+=(const Vector &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}

			const Vector operator+(T val)const {
				return Vector(x + val, y + val, z + val);
			}

			Vector operator+=(T val) {
				x += val;
				y += val;
				z += val;
				return *this;
			}

			const Vector operator-() const { return Vector(-x, -y, -z); }

			const Vector operator-(const Vector &v) const {
				return Vector(x - v.x, y - v.y, z - v.z);
			}

			Vector & operator-=(const Vector &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return *this;
			}

			const Vector operator-(T val)const {
				return Vector(x - val, y - val, z - val);
			}

			Vector operator-=(T val) {
				x -= val;
				y -= val;
				z -= val;
				return *this;
			}

			template <typename U>
			const Vector operator*(U s) const {
				return Vector(s * x, s * y, s * z);
			}

			template <typename U>
			Vector & operator*=(U s) {
				x *= s;
				y *= s;
				z *= s;
				return *this;
			}

			template <typename U>
			const Vector operator/(U f) const {
				const float inv = (float)1 / f;
				return Vector(x * inv, y * inv, z * inv);
			}

			template <typename U>
			Vector & operator/=(U f) {
				const float inv = (float)1 / f;
				x *= inv;
				y *= inv;
				z *= inv;
				return *this;
			}
		};

		template <typename U, typename T>
		const Vector<T> operator * (U a, const Vector<T> & v) {
			return v * a;
		}

		template<typename T, typename U>
		const Vector<T> operator+(U a, const Vector<T> & v) {
			return v + a;
		}

		template<typename T, typename U>
		const Vector<T> operator-(U a, const Vector<T> & v) {
			return v - a;
		}
	}

	using Vectorf = Basic::Vector<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_VECTOR_H_
