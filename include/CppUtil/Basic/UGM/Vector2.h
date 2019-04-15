#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR2_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR2_H_

#include <CppUtil/Basic/UGM/Vector.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val2.h>
#include <CppUtil/Basic/UGM/ext/Linearity_2.h>
#include <CppUtil/Basic/UGM/ext/InnerProduct_Euclidean_2.h>
#include <CppUtil/Basic/UGM/ext/Normed_InnerProduct.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<2, T> : public EXT::NI_IE_L_B<2, T, Vector<2, T>> {
		public:
			using EXT::NI_IE_L_B<2, T, Vector<2, T>>::NI_IE_L_B;

#ifndef NDEBUG
			// 仅用于 Debug 时方便在 IDE 中显示结果
		public:
			Vector & operator =(const Vector & v) {
				*static_cast<EXT::NI_IE_L_B<2, T, Vector<2, T>>*>(this) = v;
				return *this;
			}
		private:
			const T & _x{ *(((T*)this) + 0) };
			const T & _y{ *(((T*)this) + 1) };
#endif // !NDEBUG
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR2_H_
