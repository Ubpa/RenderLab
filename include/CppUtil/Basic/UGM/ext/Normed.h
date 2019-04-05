#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_NORM_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_NORM_H_

#include <CppUtil/Basic/UGM/ext/Normed_Base.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <int N, typename T, NormType NormT, typename BaseT, typename ImplT>
			class Normed : public Normed_Base<N, T, NormT, BaseT, ImplT> {
			public:
				using Normed_Base<N, T, NormT, BaseT, ImplT>::Normed_Base;

			public:
				const ImplT Normalize() const {
					return (*this) / Norm();
				}

				ImplT & NormalizeSelf() {
					return (*this) /= Norm();
				}
			};
		}
	}
}
#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_NORM_H_
