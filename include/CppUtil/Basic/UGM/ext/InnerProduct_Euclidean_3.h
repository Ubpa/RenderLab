#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_EUCLIDEAN_3_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_EUCLIDEAN_3_H_

#include <CppUtil/Basic/UGM/ext/InnerProduct.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template<typename T, typename BaseT, typename ImplT>
			class InnerProduct_Base<3, T, InnerProductType::Euclidean, BaseT, ImplT> : public BaseT {
			public:
				using BaseT::BaseT;

			public:
				static T Dot(const ImplT & lhs, const ImplT & rhs) {
					return
						lhs.x * rhs.x +
						lhs.y * rhs.y +
						lhs.z * rhs.z;
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_INNER_PRODUCT_EUCLIDEAN_3_H_
