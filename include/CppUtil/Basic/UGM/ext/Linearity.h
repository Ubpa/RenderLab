#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_VAL_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_VAL_H_

#include <CppUtil/Basic/UGM/ext/Linearity_Base.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <int N, typename T, typename BaseT, typename ImplT>
			class Linearity : public Linearity_Base<N, T, BaseT, ImplT> {
			public:
				using Linearity_Base<N, T, BaseT, ImplT>::Linearity_Base;

			public:
				static const ImplT Zero() {
					return ImplT(static_cast<T>(0));
				}

				static constexpr T Identiry() {
					return static_cast<T>(1);
				}

			public:
				friend const ImplT operator*(T k, const ImplT & v) {
					return v * k;
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_VAL_H_
