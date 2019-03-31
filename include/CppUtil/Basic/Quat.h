#ifndef _CPPUTIL_BASIC_MATH_QUAT_H_
#define _CPPUTIL_BASIC_MATH_QUAT_H_

#include <CppUtil/Basic/Val4.h>
#include <CppUtil/Basic/Vector.h>
#include <CppUtil/Basic/Point.h>

#include <CppUtil/Basic/Math.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class EulerYXZ;

		template <typename T>
		class Quat {
		public:
			Quat(): s(1), v(0) {}
			Quat(T s, const Val3<T> & v) :s(s), v(v) { }
			Quat(const Val3<T> & v) :s(0), v(v) { }
			Quat(const Quat & q) :s(q.s), v(q.v) { }

			Quat(const Vector<T> & axis, float theta) {
				Init(axis, theta);
			}

			void Init(const Vector<T> & axis, float theta) {
				v = axis.Norm();
				const float halfTheta = Math::Radians(theta) / 2.f;
				v *= sinf(halfTheta);
				s = cosf(halfTheta);
			}
			
		public:
			static const Quat RotateX(float theta) {
				return Quat(Vector<T>(1, 0, 0), theta);
			}
			static const Quat RotateY(float theta) {
				return Quat(Vector<T>(0, 1, 0), theta);
			}
			static const Quat RotateZ(float theta) {
				return Quat(Vector<T>(0, 0, 1), theta);
			}

			static const Quat Rotate(const EulerYXZ<T> & euler) {
				const auto qY = RotateY(euler.y);
				const auto axisX = qY * Point<T>(1, 0, 0);
				const Quat qX(axisX, euler.x);
				const auto axisZ = qX * (qY * Point<T>(0, 0, 1));
				const Quat qZ(axisZ, euler.z);
				return qZ * qX * qY;
			}

		public:
			const Vector<T> GetAxis() const {
				const auto sinHalfTheta = sqrt(1 - s * s);
				return v / sinHalfTheta;
			}

			T GetTheta() const {
				return Math::Degrees(static_cast<T>(2)*acos(s));
			}

			T ModularLength() const { return sqrt(v.Length2() + s * s); }

			Quat Conjugate() const { return Quat(s, -v); }

			Quat Inverse() const { return Conjugate() / ModularLength(); }

			const T Dot(const Quat & rhs) const {
				return s * rhs.s + v.Dot(rhs.v);
			}

			static const Quat SLerp(const Quat & q0, Quat q1, float t) {
				auto theta = acos(q0.Dot(q1));
				if (theta < 0) {
					q1 = -q1;
					theta = -theta;
				}

				if (theta > 0.9995f)
					return (1.f - t)*q0 + t * q1;
				else
					return (sin((1.f - t)*theta) * q0 + sin(t*theta)*q1) / sin(theta);
			}

		public:
			const Quat operator+(const Quat & rhs) const {
				return Quat(s + rhs.s, v + rhs.v);
			}

			Quat & operator+=(const Quat & rhs) {
				v += rhs.v;
				s += rhs.s;
				return *this;
			}

			const Quat operator-()const {
				return Quat(-s, -v);
			}

			const Quat operator*(const Quat & rhs) const {
				const auto rstS = s * rhs.s - v.Dot(rhs.v);
				const auto rstV = s * rhs.v + rhs.s*v + v.Cross(rhs.v);
				return Quat(rstS, rstV);
			}

			const Point<T> operator*(const Point<T> & p) const {
				return (*this * Quat(p) * Inverse()).v;
			}

			Quat & operator*=(const Quat & rhs) {
				s = s * rhs.s - v.Dot(rhs.v);
				v = s * rhs.v + rhs.s*v + v.Cross(rhs.v);
				return *this;
			}

			template <typename U>
			const Quat operator*(U k) const {
				return Quat(s*k, v*k);
			}

			template <typename U>
			Quat & operator*=(U k) const {
				s *= k;
				v *= k;
				return *this;
			}

			template <typename U>
			const Quat operator/(U k) const {
				const float invK = 1.f / static_cast<float>(k);
				return Quat(s*invK, v*invK);
			}

			template <typename U>
			Quat & operator/=(U k) {
				const float invK = 1.f / static_cast<float>(k);
				s *= invK;
				v *= invK;
				return *this;
			}

			bool operator==(const Quat & rhs) const {
				return (v == v && s == s) || (v == -v && s == -s);
			}

			bool operator!=(const Quat & rhs) const {
				return !(*this == rhs);
			}

			Quat & operator=(const Quat & rhs) {
				v = rhs.v;
				s = rhs.s;
				return *this;
			}

		public:
			union {
				struct {
					Vector<T> v;
					T s;
				};
				struct {
					T x, y, z, w;
				};
			};
		};

		template<typename T, typename U>
		const Quat<T> operator*(U k, const Quat<T> & q) {
			return q * k;
		}

		template<typename T>
		std::ostream & operator<<(std::ostream & os, const Quat<T> & q) {
			os << "[" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << "]";
			return os;
		}
	}

	using Quatf = Basic::Quat<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_QUAT_H_
