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
			//explicit Vector(const Point<T> &p) : x(p.x), y(p.y), z(p.z) { }
			//explicit Vector(const Normal<T> &n) : x(n.x), y(n.y), z(n.z) { }
			
		public:
			float Length2() const { return x * x + y * y + z * z; }
			float Length() const { return std::sqrt(Length2()); }

		public:
			static T Dot(const Vector<T> & v0, const Vector<T> & v1) {
				return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z;
			}
			T Dot(const Vector<T> & v) const {
				return Dot(*this, v);
			}

			const Vector<T> Cross(const Vector<T> & v0, const Vector<T> & v1) const {
				//  i,  j,  k
				// x0, y0, z0
				// x1, y1, z1
				return Vector<T>(v0.y*v1.z - v0.z * v1.y, v0.z*v1.x - v0.x * v1.z, v0.x*v1.y - v0.y * v1.x);
			}
			const Vector<T> Cross(const Vector<T> & v) const {
				return Cross(*this, v);
			}

			const Vector<T> Norm() const {
				return *this / Length();
			}

		public:
			const Vector<T> operator+(const Vector<T> &v) const {
				return Vector(x + v.x, y + v.y, z + v.z);
			}

			const Vector<T> &operator+=(const Vector<T> &v) {
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}

			const Vector<T> operator-(const Vector<T> &v) const {
				return Vector(x - v.x, y - v.y, z - v.z);
			}

			const Vector<T> operator-() const { return Vector<T>(-x, -y, -z); }

			const Vector<T> &operator-=(const Vector<T> &v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return *this;
			}

			template <typename U>
			const Vector<T> operator*(U s) const {
				return Vector<T>(s * x, s * y, s * z);
			}

			template <typename U>
			Vector<T> & operator*=(U s) {
				DCHECK(!isNaN(s));
				x *= s;
				y *= s;
				z *= s;
				return *this;
			}

			template <typename U>
			const Vector<T> operator/(U f) const {
				float inv = (float)1 / f;
				return Vector<T>(x * inv, y * inv, z * inv);
			}

			template <typename U>
			Vector<T> & operator/=(U f) {
				float inv = (float)1 / f;
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

		using Vectorf = Vector<float>;
		using Vectori = Vector<int>;
	}
}

#endif // !_CPPUTIL_BASIC_MATH_VECTOR_H_
