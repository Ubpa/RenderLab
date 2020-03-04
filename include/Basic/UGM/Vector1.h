#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_

#include <Basic/UGM/Vector.h>

#include <Basic/UGM/ext/Alias.h>
#include <Basic/UGM/ext/Basic_Val1.h>
#include <Basic/UGM/ext/Linearity_1.h>
#include <Basic/UGM/ext/InnerProduct_Euclidean_1.h>
#include <Basic/UGM/ext/Normed_InnerProduct.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<1, T> : public EXT::NI_IE_L_B<1, T, Vector<1,T>> {
		public:
			using EXT::NI_IE_L_B<1, T, Vector<1, T>>::NI_IE_L_B;
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_
