#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_

#include <CppUtil/Basic/UGM/Vector.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val1.h>
#include <CppUtil/Basic/UGM/ext/Metric_Euclidean_1.h>
#include <CppUtil/Basic/UGM/ext/Linearity_1.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<1, T> : public EXT::L_ME_B<1, T, Vector<1, T>> {
		public:
			using EXT::L_ME_B<1, T, Vector<1, T>>::L_ME_B;
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR1_H_
