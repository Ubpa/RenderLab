#ifndef _CPPUTIL_BASIC_MATH_QUAT_H_
#define _CPPUTIL_BASIC_MATH_QUAT_H_

#include <CppUtil/Basic/Val4.h>
#include <CppUtil/Basic/Point3.h>

#include <CppUtil/Basic/Math.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class EulerYXZ;

		template <typename T>
		class Quat : public Val<4, T>{
		public:
			template<typename U, typename V>
			Quat(U real, const Val<3, V> & imag) : Val<4, T>(imag, real) { }
			
			Quat() : Quat(1, Val<3, T>(0)) { }
			
			Quat(T real) : Quat(real, Val<3, T>(0)) { }

			template<typename U>
			Quat(const Val<3, U> & imag) : Quat(0, imag) { }

			template<typename U>
			Quat(const Val<4, U> & val4) : Quat(val4.real, val4.imag) { }

			template<typename U, typename V>
			Quat(const Vector<3, U> & axis, V theta) {
				Init(axis, static_cast<float>(theta));
			}

			template<typename U>
			void Init(const Vector<3, U> & axis, float theta) {
				imag = static_cast<Vector<3,T>>(axis.Norm());
				const T halfTheta = Math::Radians(theta) / static_cast<T>(2);
				imag *= sin(halfTheta);
				real = cos(halfTheta);
			}
			
		public:
			enum Axis { X, Y, Z };
			static const Quat Rotate(Axis axis, float theta) {
				switch (axis) {
				case X:
					return RotateX(theta);
				case Y:
					return RotateY(theta);
				case Z:
					return RotateZ(theta);
				}
			}
			static const Quat RotateX(float theta) {
				return Quat(Vector<3, T>(1, 0, 0), theta);
			}
			static const Quat RotateY(float theta) {
				return Quat(Vector<3, T>(0, 1, 0), theta);
			}
			static const Quat RotateZ(float theta) {
				return Quat(Vector<3, T>(0, 0, 1), theta);
			}

			static const Quat Rotate(const EulerYXZ<T> & euler) {
				const auto qY = RotateY(euler.y);
				const auto axisX = qY * Point<3, T>(1, 0, 0);
				const Quat qX(Vector<3,T>(axisX), euler.x);
				const auto axisZ = qX * (qY * Point<3, T>(0, 0, 1));
				const Quat qZ(Vector<3, T>(axisZ), euler.z);
				return qZ * qX * qY;
			}

		public:
			const Vector<3, T> GetAxis() const {
				const auto sinHalfTheta = sqrt(static_cast<T>(1) - real * real);
				return sinHalfTheta == 0 ? Vector<3, T>(0) : imag / sinHalfTheta;
			}

			T GetTheta() const {
				return Math::Degrees(static_cast<T>(2)*acos(real));
			}

			bool IsIdentity() const {
				return imag.IsZero() && abs(real) == 1;
			}

			T ModularLength() const { return sqrt(imag.Length2() + real * real); }

			Quat Conjugate() const { return Quat(real, -imag); }

			Quat Inverse() const { return Conjugate() / ModularLength(); }

			const T Dot(const Quat & rhs) const {
				return real * rhs.real + imag.Dot(rhs.imag);
			}

			static const Quat SLerp(const Quat & q0, Quat q1, float t) {
				auto theta = acos(q0.Dot(q1));
				if (theta < 0) {
					q1 = -q1;
					theta = -theta;
				}

				if (theta > static_cast<T>(0.9995))
					return (static_cast<T>(1) - t) * q0 + t * q1;
				else
					return (sin((static_cast<T>(1) - t)*theta) * q0 + sin(t*theta)*q1) / sin(theta);
			}

		public:
			const Quat operator+(const Quat & rhs) const {
				return Quat(real + rhs.real, imag + rhs.imag);
			}

			Quat & operator+=(const Quat & rhs) {
				imag += rhs.imag;
				real += rhs.real;
				return *this;
			}

			const Quat operator-()const {
				return Quat(-real, -imag);
			}

			const Quat operator*(const Quat & rhs) const {
				const auto rstS = real * rhs.real - imag.Dot(rhs.imag);
				const auto rstV = real * rhs.imag + rhs.real*imag + imag.Cross(rhs.imag);
				return Quat(rstS, rstV);
			}

			const Point<3, T> operator*(const Point<3, T> & p) const {
				return (*this * Quat(p) * Inverse()).imag;
			}

			Quat & operator*=(const Quat & rhs) {
				real = real * rhs.real - imag.Dot(rhs.imag);
				imag = real * rhs.imag + rhs.real*imag + imag.Cross(rhs.imag);
				return *this;
			}

			template <typename U>
			const Quat operator*(U k) const {
				return Quat(real*k, imag*k);
			}

			template <typename U>
			Quat & operator*=(U k) const {
				real *= k;
				imag *= k;
				return *this;
			}

			template <typename U>
			const Quat operator/(U k) const {
				const float invK = 1.f / static_cast<float>(k);
				return Quat(real*invK, imag*invK);
			}

			template <typename U>
			Quat & operator/=(U k) {
				const float invK = 1.f / static_cast<float>(k);
				real *= invK;
				imag *= invK;
				return *this;
			}

			bool operator==(const Quat & rhs) const {
				return (imag == imag && real == real) || (imag == -imag && real == -real);
			}

			bool operator!=(const Quat & rhs) const {
				return !(*this == rhs);
			}

			Quat & operator=(const Quat & rhs) {
				imag = rhs.imag;
				real = rhs.real;
				return *this;
			}
		};

		template<typename T, typename U>
		const Quat<T> operator*(U k, const Quat<T> & q) {
			return q * k;
		}
	}

	template<typename T>
	using Quat = Basic::Quat<T>;

	using Quatf = Basic::Quat<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_QUAT_H_
