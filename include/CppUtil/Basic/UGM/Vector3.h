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
		class Vector<3, T> : public EXT::NI_C_IE_L_B_3<T, Vector<3, T>> {
		public:
			using EXT::NI_C_IE_L_B_3<T, Vector<3, T>>::NI_C_IE_L_B_3;

#ifndef NDEBUG
		// 仅用于 Debug 时方便在 IDE 中显示结果
		public:
			Vector & operator =(const Vector & v) {
				*static_cast<EXT::NI_C_IE_L_B_3<T, Vector<3, T>>*>(this) = v;
				return *this;
			}
		private:
			const T & _x{ *(((T*)this)+0) };
			const T & _y{ *(((T*)this)+1) };
			const T & _z{ *(((T*)this)+2) };
#endif // !NDEBUG
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR3_H_
