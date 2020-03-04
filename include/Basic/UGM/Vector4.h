#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_

#include <Basic/UGM/Vector.h>

#include <Basic/UGM/ext/Alias.h>
#include <Basic/UGM/ext/Basic_Val4.h>
#include <Basic/UGM/ext/Linearity_4.h>
#include <Basic/UGM/ext/InnerProduct_Euclidean_4.h>
#include <Basic/UGM/ext/Normed_InnerProduct.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<4, T> : public EXT::NI_IE_L_B<4, T, Vector<4, T>> {
		public:
			using EXT::NI_IE_L_B<4, T, Vector<4, T>>::NI_IE_L_B;
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_
