#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR3_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR3_H_

#include <CppUtil/Basic/UGM/Vector.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val3.h>
#include <CppUtil/Basic/UGM/ext/Linearity_3.h>
#include <CppUtil/Basic/UGM/ext/InnerProduct_Euclidean_3.h>
#include <CppUtil/Basic/UGM/ext/Normed_InnerProduct.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<3, T> : public EXT::NI_IE_L_B<3, T, Vector<3, T>> {
		public:
			using EXT::NI_IE_L_B<3, T, Vector<3, T>>::NI_IE_L_B;

		public:
			static const Vector Cross(const Vector & lhs, Vector & rhs) {
				//  i  j  k
				// x0 y0 z0
				// x1 y1 z1
				return Vector(
					lhs.y*rhs.z - lhs.z*rhs.y,
					lhs.z*rhs.x - lhs.x*rhs.z,
					lhs.x*rhs.y - lhs.y*rhs.x
				);
			}

			const Vector Cross(const Vector & v) const {
				return Cross(*this, v);
			}
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR3_H_
