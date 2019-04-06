#ifndef _CPPUTIL_BASIC_MATH_UGM_SCALE_H_
#define _CPPUTIL_BASIC_MATH_UGM_SCALE_H_

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val3.h>
#include <CppUtil/Basic/UGM/ext/Linearity_3.h>
#include <CppUtil/Basic/UGM/ext/HadamardProduct_3.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Scale : public EXT::H_L_B<3, T, Scale<T>> {
		public:
			using EXT::H_L_B<3, T, Scale<T>>::H_L_B;
		};
	}

	template<typename T>
	using Scale = Basic::Scale<T>;

	using Scalef = Scale<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_SCALE_H_
