#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_HADAMARD_PRODUCT_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_HADAMARD_PRODUCT_H_

#include <Basic/UGM/ext/HadamardProduct_Base.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template<int N, typename T, typename BaseT, typename ImplT>
			class HadamardProduct : public HadamardProduct_Base<N,T,BaseT,ImplT> {
			public:
				using HadamardProduct_Base<N, T, BaseT, ImplT>::HadamardProduct_Base;

			public:
				static const ImplT Indentity_HadamardProduct() {
					return ImplT(static_cast<T>(1));
				}

				const ImplT Inverse_HadamardProduct() const {
					return Indentity_HadamardProduct() / ToImplT();
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_HADAMARD_PRODUCT_H_
