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

#ifndef NDEBUG
			// 仅用于 Debug 时方便在 IDE 中显示结果
		public:
			Scale & operator =(const Scale & v) {
				*static_cast<EXT::H_L_B<3, T, Scale<T>>*>(this) = v;
				return *this;
			}
		private:
			const T & _x{ *(((T*)this) + 0) };
			const T & _y{ *(((T*)this) + 1) };
			const T & _z{ *(((T*)this) + 2) };
#endif // !NDEBUG
		};
	}

	template<typename T>
	using Scale = Basic::Scale<T>;

	using Scalef = Scale<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_SCALE_H_
