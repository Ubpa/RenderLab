#ifndef _CPPUTIL_BASIC_MATH_MAT_4X4_H_
#define _CPPUTIL_BASIC_MATH_MAT_4X4_H_

#include <CppUtil/Basic/Error.h>
#include <CppUtil/Basic/Vector4.h>

#include <iostream>
#include <algorithm>

namespace CppUtil {
	namespace Basic {
		// 内部存储为列主序
		template<typename T>
		class Mat4x4 {
		public:
			using type = Mat4x4;

		public:
			Mat4x4(T d0, T d1, T d2, T d3)
				: m{ {d0, static_cast<T>(0),static_cast<T>(0),static_cast<T>(0)},
			{static_cast<T>(0),d1,static_cast<T>(0),static_cast<T>(0)},
			{static_cast<T>(0),static_cast<T>(0),d2,static_cast<T>(0)},
			{static_cast<T>(0),static_cast<T>(0),static_cast<T>(0),d3} } { }

			explicit Mat4x4(T d = static_cast<T>(1))
				: Mat4x4(d, d, d, d) { }

			// mat 为列主序
			explicit Mat4x4(const T mat[4][4]) { memcpy(m, mat, 16 * sizeof(T)); }

			Mat4x4(
				T t00, T t01, T t02, T t03,
				T t10, T t11, T t12, T t13,
				T t20, T t21, T t22, T t23,
				T t30, T t31, T t32, T t33
			) : m{
			{t00, t10, t20, t30},
			{t01, t11, t21, t31},
			{t02, t12, t22, t32},
			{t03, t13, t23, t33} } { }

		public:
			const Vector4<T> & GetCol(int i) const { return m[i]; }
			Vector4<T> & GetCol(int i) { return m[i]; }

			// 列主序
			T * Data() { return &(m[0][0]); }

			// 列主序
			const T * Data() const { return const_cast<type*>(this)->Data(); }


		public:
			bool IsIdentity() const {
				static constexpr T one = static_cast<T>(1);
				static constexpr T zero = static_cast<T>(0);

				return (m[0][0] == one && m[0][1] == zero && m[0][2] == zero && m[0][3] == zero
					&& m[1][0] == zero && m[1][1] == one && m[1][2] == zero && m[1][3] == zero
					&& m[2][0] == zero && m[2][1] == zero && m[2][2] == one && m[2][3] == zero
					&& m[3][0] == zero && m[3][1] == zero && m[3][2] == zero && m[3][3] == one);
			}

			T Tr() const { return m[0][0] + m[1][1] + m[2][2] + m[3][3]; }

			Mat4x4 Transpose() const {
				return Mat4x4(m[0][0], m[1][0], m[2][0], m[3][0], m[0][1],
					m[1][1], m[2][1], m[3][1], m[0][2], m[1][2],
					m[2][2], m[3][2], m[0][3], m[1][3], m[2][3],
					m[3][3]);
			}

			Mat4x4 Inverse() const {
				static const auto & ERROR = ErrorRetVal(&Mat4x4::Inverse);

				int indxc[4], indxr[4];
				int ipiv[4] = { 0, 0, 0, 0 };
				T minv[4][4];
				memcpy(minv, m, 4 * 4 * sizeof(T));
				for (int i = 0; i < 4; i++) {
					int irow = 0, icol = 0;
					T big = static_cast<T>(0);
					// Choose pivot
					for (int j = 0; j < 4; j++) {
						if (ipiv[j] != 1) {
							for (int k = 0; k < 4; k++) {
								if (ipiv[k] == 0) {
									if (std::abs(minv[k][j]) >= big) {
										big = static_cast<T>(std::abs(minv[k][j]));
										irow = j;
										icol = k;
									}
								}
								else if (ipiv[k] > 1)
									return ERROR;
							}
						}
					}
					++ipiv[icol];
					// Swap rows _irow_ and _icol_ for pivot
					if (irow != icol) {
						for (int k = 0; k < 4; ++k) std::swap(minv[k][irow], minv[k][icol]);
					}
					indxr[i] = irow;
					indxc[i] = icol;
					if (minv[icol][icol] == static_cast<T>(0))
						return ERROR;

					// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
					T pivinv = static_cast<T>(1) / minv[icol][icol];
					minv[icol][icol] = 1.;
					for (int j = 0; j < 4; j++) minv[j][icol] *= pivinv;

					// Subtract this row from others to zero out their columns
					for (int j = 0; j < 4; j++) {
						if (j != icol) {
							T save = minv[icol][j];
							minv[icol][j] = 0;
							for (int k = 0; k < 4; k++) minv[k][j] -= minv[k][icol] * save;
						}
					}
				}
				// Swap columns to reflect permutation
				for (int j = 3; j >= 0; j--) {
					if (indxr[j] != indxc[j]) {
						for (int k = 0; k < 4; k++)
							std::swap(minv[indxr[j]][k], minv[indxc[j]][k]);
					}
				}
				return Mat4x4(minv);
			}

		public:
			bool operator ==(const Mat4x4 & rhs) const {
				return
					m[0][0] == rhs.m[0][0]
					&& m[0][1] == rhs.m[0][1]
					&& m[0][2] == rhs.m[0][2]
					&& m[0][3] == rhs.m[0][3]
					&& m[1][0] == rhs.m[1][0]
					&& m[1][1] == rhs.m[1][1]
					&& m[1][2] == rhs.m[1][2]
					&& m[1][3] == rhs.m[1][3]
					&& m[2][0] == rhs.m[2][0]
					&& m[2][1] == rhs.m[2][1]
					&& m[2][2] == rhs.m[2][2]
					&& m[2][3] == rhs.m[2][3]
					&& m[3][0] == rhs.m[3][0]
					&& m[3][1] == rhs.m[3][1]
					&& m[3][2] == rhs.m[3][2]
					&& m[3][3] == rhs.m[3][3];
			}

			bool operator!=(const Mat4x4 & rhs) const {
				return
					m[0][0] != rhs.m[0][0]
					|| m[0][1] != rhs.m[0][1]
					|| m[0][2] != rhs.m[0][2]
					|| m[0][3] != rhs.m[0][3]
					|| m[1][0] != rhs.m[1][0]
					|| m[1][1] != rhs.m[1][1]
					|| m[1][2] != rhs.m[1][2]
					|| m[1][3] != rhs.m[1][3]
					|| m[2][0] != rhs.m[2][0]
					|| m[2][1] != rhs.m[2][1]
					|| m[2][2] != rhs.m[2][2]
					|| m[2][3] != rhs.m[2][3]
					|| m[3][0] != rhs.m[3][0]
					|| m[3][1] != rhs.m[3][1]
					|| m[3][2] != rhs.m[3][2]
					|| m[3][3] != rhs.m[3][3];
			}

			const Mat4x4 operator*(const Mat4x4 & rhs) const {
				const auto & lhs = *this;
				T t00 = lhs(0, 0) * rhs(0, 0) + lhs(0, 1) * rhs(1, 0) + lhs(0, 2) * rhs(2, 0) + lhs(0, 3) * rhs(3, 0);
				T t01 = lhs(0, 0) * rhs(0, 1) + lhs(0, 1) * rhs(1, 1) + lhs(0, 2) * rhs(2, 1) + lhs(0, 3) * rhs(3, 1);
				T t02 = lhs(0, 0) * rhs(0, 2) + lhs(0, 1) * rhs(1, 2) + lhs(0, 2) * rhs(2, 2) + lhs(0, 3) * rhs(3, 2);
				T t03 = lhs(0, 0) * rhs(0, 3) + lhs(0, 1) * rhs(1, 3) + lhs(0, 2) * rhs(2, 3) + lhs(0, 3) * rhs(3, 3);
				T t10 = lhs(1, 0) * rhs(0, 0) + lhs(1, 1) * rhs(1, 0) + lhs(1, 2) * rhs(2, 0) + lhs(1, 3) * rhs(3, 0);
				T t11 = lhs(1, 0) * rhs(0, 1) + lhs(1, 1) * rhs(1, 1) + lhs(1, 2) * rhs(2, 1) + lhs(1, 3) * rhs(3, 1);
				T t12 = lhs(1, 0) * rhs(0, 2) + lhs(1, 1) * rhs(1, 2) + lhs(1, 2) * rhs(2, 2) + lhs(1, 3) * rhs(3, 2);
				T t13 = lhs(1, 0) * rhs(0, 3) + lhs(1, 1) * rhs(1, 3) + lhs(1, 2) * rhs(2, 3) + lhs(1, 3) * rhs(3, 3);
				T t20 = lhs(2, 0) * rhs(0, 0) + lhs(2, 1) * rhs(1, 0) + lhs(2, 2) * rhs(2, 0) + lhs(2, 3) * rhs(3, 0);
				T t21 = lhs(2, 0) * rhs(0, 1) + lhs(2, 1) * rhs(1, 1) + lhs(2, 2) * rhs(2, 1) + lhs(2, 3) * rhs(3, 1);
				T t22 = lhs(2, 0) * rhs(0, 2) + lhs(2, 1) * rhs(1, 2) + lhs(2, 2) * rhs(2, 2) + lhs(2, 3) * rhs(3, 2);
				T t23 = lhs(2, 0) * rhs(0, 3) + lhs(2, 1) * rhs(1, 3) + lhs(2, 2) * rhs(2, 3) + lhs(2, 3) * rhs(3, 3);
				T t30 = lhs(3, 0) * rhs(0, 0) + lhs(3, 1) * rhs(1, 0) + lhs(3, 2) * rhs(2, 0) + lhs(3, 3) * rhs(3, 0);
				T t31 = lhs(3, 0) * rhs(0, 1) + lhs(3, 1) * rhs(1, 1) + lhs(3, 2) * rhs(2, 1) + lhs(3, 3) * rhs(3, 1);
				T t32 = lhs(3, 0) * rhs(0, 2) + lhs(3, 1) * rhs(1, 2) + lhs(3, 2) * rhs(2, 2) + lhs(3, 3) * rhs(3, 2);
				T t33 = lhs(3, 0) * rhs(0, 3) + lhs(3, 1) * rhs(1, 3) + lhs(3, 2) * rhs(2, 3) + lhs(3, 3) * rhs(3, 3);
				return Mat4x4(
					t00, t01, t02, t03,
					t10, t11, t12, t13,
					t20, t21, t22, t23,
					t30, t31, t32, t33
				);
			}

			T operator()(int row, int col) const {
				return const_cast<type*>(this)->operator()(row, col);
			}

			T & operator()(int row, int col) {
				return m[col][row];
			}

			friend std::ostream & operator<<(std::ostream & os, const Mat4x4 & mat) {
				os << "[" << mat(0, 0) << ", " << mat(0, 1) << ", " << mat(0, 2) << ", " << mat(0, 3) << endl;
				os << mat(1, 0) << ", " << mat(1, 1) << ", " << mat(1, 2) << ", " << mat(1, 3) << endl;
				os << mat(2, 0) << ", " << mat(2, 1) << ", " << mat(2, 2) << ", " << mat(2, 3) << endl;
				os << mat(3, 0) << ", " << mat(3, 1) << ", " << mat(3, 2) << ", " << mat(3, 3) << "]" << endl;
				return os;
			}

		private:
			// 列主序
			// m[i] 为第 i 列
			// m[i][j] 为第 j 行第 j 列
			Vector4<T> m[4];
		};

		using Mat4f = Mat4x4<float>;
		// using Mat4d = Mat4x4<double>;
	}

}

#endif // !_CPPUTIL_BASIC_MATH_MAT_4X4_H_
