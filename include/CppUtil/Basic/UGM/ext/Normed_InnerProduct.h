#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_NORM_INNER_PRODUCT_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_NORM_INNER_PRODUCT_H_

#include <CppUtil/Basic/UGM/ext/Normed.h>
#include <cassert>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <int N, typename T, typename BaseT, typename ImplT>
			class Normed_Base<N, T, NormType::InnerProduct, BaseT, ImplT> : public BaseT {
			public:
				using BaseT::BaseT;

			public:
				T Norm2() const {
					return Dot(*static_cast<const ImplT*>(this));
				}

				T Norm() const {
					return sqrt(Norm2());
				}

				static T CosTheta(const ImplT & lhs, const ImplT & rhs) {
					assert(lhs.Normalize() * rhs.Normalize() != static_cast<T>(0));
					return Dot(lhs, rhs) / (lhs.Normalize() * rhs.Normalize());
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_NORM_INNER_PRODUCT_H_
