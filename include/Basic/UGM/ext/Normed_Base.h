#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_NORM_BASE_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_NORM_BASE_H_

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			enum class NormType {
				InnerProduct, // 要求子类是内积空间
			};

			// 赋范（线性）空间
			// BaseT 是一个线性空间
			template <int N, typename T, NormType normType, typename BaseT, typename ImplT>
			class Normed_Base;
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_NORM_BASE_H_
