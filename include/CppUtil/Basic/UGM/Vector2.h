#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR2_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR2_H_

#include <CppUtil/Basic/UGM/Vector.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val2.h>
#include <CppUtil/Basic/UGM/ext/Metric_Euclidean_2.h>
#include <CppUtil/Basic/UGM/ext/Linearity_2.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<2, T> : public EXT::L_ME_B<2,T,Vector<2,T>> {
		public:
			using EXT::L_ME_B<2, T, Vector<2, T>>::L_ME_B;
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR2_H_
