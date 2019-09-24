#ifndef _CPPUTIL_BASIC_MATH_UGM_MAT_3X3_H_
#define _CPPUTIL_BASIC_MATH_UGM_MAT_3X3_H_

#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/Vector3.h>

#include <CppUtil/Basic/Error.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Normal;

		template <typename T>
		class Mat4x4;

		// 内部存储为列主序
		template<typename T>
		class Mat3x3 {
		public:
			using type = Mat3x3;

		public:
			Mat3x3(T d0, T d1, T d2)
				: m{ {d0, static_cast<T>(0),static_cast<T>(0)},
			{static_cast<T>(0),d1,static_cast<T>(0)},
			{static_cast<T>(0),static_cast<T>(0),d2} } { }

			explicit Mat3x3(T d = static_cast<T>(1))
				: Mat3x3(d, d, d) { }

			// mat 为列主序
			explicit Mat3x3(const T mat[3][3]) { memcpy(m, mat, 9 * sizeof(T)); }

			Mat3x3(const Val<3, T> & col0, const Val<3, T> & col1, const Val<3, T> & col2)
				:m{ col0, col1, col2 } { }

			Mat3x3(
				T t00, T t01, T t02,
				T t10, T t11, T t12,
				T t20, T t21, T t22
			) : m{
				{t00, t10, t20}, // col 0
				{t01, t11, t21}, // col 1
				{t02, t12, t22}  // col 2
			} { }

			Mat3x3(const Mat4x4<T> & mat)
			: m{
				{mat(0,0),mat(1,0),mat(2,0)}, // col 0
				{mat(0,1),mat(1,1),mat(2,1)}, // col 1
				{mat(0,2),mat(1,2),mat(2,2)}  // col 2
			} { }
			
		public:
			const Vector<3, T> & GetCol(int i) const { return m[i]; }
			Vector<3, T> & GetCol(int i) { return m[i]; }

		public:
			bool IsIdentity() const {
				static constexpr T one = static_cast<T>(1);
				static constexpr T zero = static_cast<T>(0);

				return (m[0][0] == one && m[0][1] == zero && m[0][2] == zero
					&& m[1][0] == zero && m[1][1] == one && m[1][2] == zero
					&& m[2][0] == zero && m[2][1] == zero && m[2][2] == one);
			}

			T Tr() const { return m[0][0] + m[1][1] + m[2][2]; }

			Mat3x3 Transpose() const {
				const auto & mat = *this;
				return Mat3x3(
					mat(0, 0), mat(1, 0), mat(2, 0),
					mat(0, 1), mat(1, 1), mat(2, 1),
					mat(0, 2), mat(1, 2), mat(2, 2)
				);
			}

			Mat3x3 Inverse() const {
				static const auto & ERROR = ErrorRetVal(&Mat3x3::Inverse);
				
				T OneOverDeterminant = static_cast<T>(1) / (
					+m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
					- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
					+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

				Mat3x3 Inverse;
				Inverse.m[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * OneOverDeterminant;
				Inverse.m[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * OneOverDeterminant;
				Inverse.m[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * OneOverDeterminant;
				Inverse.m[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * OneOverDeterminant;
				Inverse.m[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * OneOverDeterminant;
				Inverse.m[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * OneOverDeterminant;
				Inverse.m[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * OneOverDeterminant;
				Inverse.m[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * OneOverDeterminant;
				Inverse.m[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * OneOverDeterminant;

				return Inverse;
			}

			// 列主序
			T * Data() { return &(m[0][0]); }

			// 列主序
			const T * Data() const { return const_cast<type*>(this)->Data(); }

		public:
			bool operator==(const Mat3x3 & rhs) const {
				return
					m[0][0] == rhs.m[0][0]
					&& m[0][1] == rhs.m[0][1]
					&& m[0][2] == rhs.m[0][2]
					&& m[1][0] == rhs.m[1][0]
					&& m[1][1] == rhs.m[1][1]
					&& m[1][2] == rhs.m[1][2]
					&& m[2][0] == rhs.m[2][0]
					&& m[2][1] == rhs.m[2][1]
					&& m[2][2] == rhs.m[2][2];
			}

			bool operator!=(const Mat3x3 & rhs) const {
				return
					m[0][0] != rhs.m[0][0]
					|| m[0][1] != rhs.m[0][1]
					|| m[0][2] != rhs.m[0][2]
					|| m[1][0] != rhs.m[1][0]
					|| m[1][1] != rhs.m[1][1]
					|| m[1][2] != rhs.m[1][2]
					|| m[2][0] != rhs.m[2][0]
					|| m[2][1] != rhs.m[2][1]
					|| m[2][2] != rhs.m[2][2];
			}

			const Mat3x3 operator*(const Mat3x3 & rhs) const {
				const auto & lhs = *this;
				T t00 = lhs(0, 0) * rhs(0, 0) + lhs(0, 1) * rhs(1, 0) + lhs(0, 2) * rhs(2, 0);
				T t01 = lhs(0, 0) * rhs(0, 1) + lhs(0, 1) * rhs(1, 1) + lhs(0, 2) * rhs(2, 1);
				T t02 = lhs(0, 0) * rhs(0, 2) + lhs(0, 1) * rhs(1, 2) + lhs(0, 2) * rhs(2, 2);
				T t10 = lhs(1, 0) * rhs(0, 0) + lhs(1, 1) * rhs(1, 0) + lhs(1, 2) * rhs(2, 0);
				T t11 = lhs(1, 0) * rhs(0, 1) + lhs(1, 1) * rhs(1, 1) + lhs(1, 2) * rhs(2, 1);
				T t12 = lhs(1, 0) * rhs(0, 2) + lhs(1, 1) * rhs(1, 2) + lhs(1, 2) * rhs(2, 2);
				T t20 = lhs(2, 0) * rhs(0, 0) + lhs(2, 1) * rhs(1, 0) + lhs(2, 2) * rhs(2, 0);
				T t21 = lhs(2, 0) * rhs(0, 1) + lhs(2, 1) * rhs(1, 1) + lhs(2, 2) * rhs(2, 1);
				T t22 = lhs(2, 0) * rhs(0, 2) + lhs(2, 1) * rhs(1, 2) + lhs(2, 2) * rhs(2, 2);
				return Mat3x3(
					t00, t01, t02,
					t10, t11, t12,
					t20, t21, t22
				);
			}

			Mat3x3 & operator*=(const Mat3x3 & rhs) {
				auto & lhs = *this;
				lhs(0, 0) = lhs(0, 0) * rhs(0, 0) + lhs(0, 1) * rhs(1, 0) + lhs(0, 2) * rhs(2, 0);
				lhs(0, 1) = lhs(0, 0) * rhs(0, 1) + lhs(0, 1) * rhs(1, 1) + lhs(0, 2) * rhs(2, 1);
				lhs(0, 2) = lhs(0, 0) * rhs(0, 2) + lhs(0, 1) * rhs(1, 2) + lhs(0, 2) * rhs(2, 2);
				lhs(1, 0) = lhs(1, 0) * rhs(0, 0) + lhs(1, 1) * rhs(1, 0) + lhs(1, 2) * rhs(2, 0);
				lhs(1, 1) = lhs(1, 0) * rhs(0, 1) + lhs(1, 1) * rhs(1, 1) + lhs(1, 2) * rhs(2, 1);
				lhs(1, 2) = lhs(1, 0) * rhs(0, 2) + lhs(1, 1) * rhs(1, 2) + lhs(1, 2) * rhs(2, 2);
				lhs(2, 0) = lhs(2, 0) * rhs(0, 0) + lhs(2, 1) * rhs(1, 0) + lhs(2, 2) * rhs(2, 0);
				lhs(2, 1) = lhs(2, 0) * rhs(0, 1) + lhs(2, 1) * rhs(1, 1) + lhs(2, 2) * rhs(2, 1);
				lhs(2, 2) = lhs(2, 0) * rhs(0, 2) + lhs(2, 1) * rhs(1, 2) + lhs(2, 2) * rhs(2, 2);
				return *this;
			}

			Mat3x3 & MulTo(Mat3x3 & rhs) const {
				const auto & lhs = *this;
				rhs(0, 0) = lhs(0, 0) * rhs(0, 0) + lhs(0, 1) * rhs(1, 0) + lhs(0, 2) * rhs(2, 0);
				rhs(0, 1) = lhs(0, 0) * rhs(0, 1) + lhs(0, 1) * rhs(1, 1) + lhs(0, 2) * rhs(2, 1);
				rhs(0, 2) = lhs(0, 0) * rhs(0, 2) + lhs(0, 1) * rhs(1, 2) + lhs(0, 2) * rhs(2, 2);
				rhs(1, 0) = lhs(1, 0) * rhs(0, 0) + lhs(1, 1) * rhs(1, 0) + lhs(1, 2) * rhs(2, 0);
				rhs(1, 1) = lhs(1, 0) * rhs(0, 1) + lhs(1, 1) * rhs(1, 1) + lhs(1, 2) * rhs(2, 1);
				rhs(1, 2) = lhs(1, 0) * rhs(0, 2) + lhs(1, 1) * rhs(1, 2) + lhs(1, 2) * rhs(2, 2);
				rhs(2, 0) = lhs(2, 0) * rhs(0, 0) + lhs(2, 1) * rhs(1, 0) + lhs(2, 2) * rhs(2, 0);
				rhs(2, 1) = lhs(2, 0) * rhs(0, 1) + lhs(2, 1) * rhs(1, 1) + lhs(2, 2) * rhs(2, 1);
				rhs(2, 2) = lhs(2, 0) * rhs(0, 2) + lhs(2, 1) * rhs(1, 2) + lhs(2, 2) * rhs(2, 2);
				return rhs;
			}

			const Val<3, T> operator*(const Val<3, T> & val3) const {
				const auto & mat = *this;
				const T x = mat(0, 0)*val3.x + mat(0, 1)*val3.y + mat(0, 2)*val3.z;
				const T y = mat(1, 0)*val3.x + mat(1, 1)*val3.y + mat(1, 2)*val3.z;
				const T z = mat(2, 0)*val3.x + mat(2, 1)*val3.y + mat(2, 2)*val3.z;
				return Val<3, T>(x, y, z);
			}

			const Point<3, T> operator*(const Point<3, T> & p) const {
				return (*this)*(static_cast<Val<3, T>>(p));
			}

			const Vector<3, T> operator*(const Vector<3, T> & v) const {
				return (*this)*(static_cast<Val<3, T>>(v));
			}

			const Normal<T> operator*(const Normal<T> & n) const {
				return (*this)*(static_cast<Val<3, T>>(n));
			}

			T operator()(int row, int col) const {
				return const_cast<type*>(this)->operator()(row, col);
			}

			T & operator()(int row, int col) {
				return m[col][row];
			}

			friend std::ostream & operator<<(std::ostream & os, const Mat3x3 & mat) {
				os << "["
					<< Math::ToZero(mat(0, 0)) << ", " << Math::ToZero(mat(0, 1)) << ", " << Math::ToZero(mat(0, 2)) << endl
					<< Math::ToZero(mat(1, 0)) << ", " << Math::ToZero(mat(1, 1)) << ", " << Math::ToZero(mat(1, 2)) << endl
					<< Math::ToZero(mat(2, 0)) << ", " << Math::ToZero(mat(2, 1)) << ", " << Math::ToZero(mat(2, 2))
					<< "]";
				return os;
			}

		private:
			// 列主序
			// m[i] 为第 i 列
			// m[i][j] 为第 j 行第 j 列
			Vector<3, T> m[3];
		};
	}

	using Mat3f = Basic::Mat3x3<float>;

}

#endif // !_CPPUTIL_BASIC_MATH_UGM_MAT_3X3_H_
