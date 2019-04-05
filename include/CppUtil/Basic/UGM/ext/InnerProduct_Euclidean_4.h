#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_EUCLIDEAN_4_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_EUCLIDEAN_4_H_

#include <CppUtil/Basic/UGM/ext/InnerProduct.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template<typename T, typename BaseT, typename ImplT>
			class InnerProduct_Base<4, T, InnerProductType::Euclidean, BaseT, ImplT> : public BaseT {
			public:
				using BaseT::BaseT;

			public:
				static const ImplT Dot(const ImplT & lhs, const ImplT & rhs) {
					return
						lhs.x * rhs.x
						lhs.y * rhs.y
						lhs.z * rhs.z
						lhs.w * rhs.w;
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_EUCLIDEAN_4_H_
