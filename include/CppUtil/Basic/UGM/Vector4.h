#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_

#include <CppUtil/Basic/UGM/Vector.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val4.h>
#include <CppUtil/Basic/UGM/ext/Metric_Euclidean_4.h>
#include <CppUtil/Basic/UGM/ext/Linearity_4.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<4, T> : public EXT::L_ME_B<4, T, Vector<4, T>> {
		public:
			using EXT::L_ME_B<4, T, Vector<4, T>>::L_ME_B;
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR4_H_
