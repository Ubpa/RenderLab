#ifndef _CPPUTIL_BASIC_MATH_UGM_VECTOR3_H_
#define _CPPUTIL_BASIC_MATH_UGM_VECTOR3_H_

#include <CppUtil/Basic/UGM/Vector.h>

#include <CppUtil/Basic/UGM/ext/Alias.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val3.h>
#include <CppUtil/Basic/UGM/ext/Metric_Euclidean_3.h>
#include <CppUtil/Basic/UGM/ext/Linearity_3.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Vector<3, T> : public EXT::L_ME_B<3, T, Vector<3, T>> {
		public:
			using EXT::L_ME_B<3, T, Vector<3, T>>::L_ME_B;
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VECTOR3_H_
