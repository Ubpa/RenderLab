#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_

#include <CppUtil/Basic/UGM/Vector.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val1.h>
#include <CppUtil/Basic/UGM/ext/Linearity_1.h>
#include <CppUtil/Basic/UGM/ext/InnerProduct_Euclidean_1.h>
#include <CppUtil/Basic/UGM/ext/Normed_InnerProduct.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<1, T> : public EXT::NI_IE_L_B<1, T, Vector<1,T>> {
		public:
			using EXT::NI_IE_L_B<1, T, Vector<1, T>>::NI_IE_L_B;

#ifndef NDEBUG
			// 仅用于 Debug 时方便在 IDE 中显示结果
		public:
			Vector & operator =(const Vector & v) {
				*static_cast<EXT::NI_IE_L_B<1, T, Vector<1, T>>*>(this) = v;
				return *this;
			}
		private:
			const T & _x{ *(((T*)this) + 0) };
#endif // !NDEBUG
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_
