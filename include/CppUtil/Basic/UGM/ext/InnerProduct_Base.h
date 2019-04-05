#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_BASE_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_BASE_H_

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			enum InnerProductType {
				Euclidean,
			};

			template<int N, typename T, InnerProductType innerProductType, typename BaseT, typename ImplT>
			class InnerProduct_Base;
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_BASE_H_
