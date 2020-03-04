#ifndef _CPPUTIL_BASIC_MATH_UGM_RGB_H_
#define _CPPUTIL_BASIC_MATH_UGM_RGB_H_

#include <Basic/UGM/ext/Alias.h>
#include <Basic/UGM/ext/Basic_Val3.h>
#include <Basic/UGM/ext/Linearity_3.h>
#include <Basic/UGM/ext/HadamardProduct_3.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class RGBA;

		template<typename T>
		class RGB : public EXT::H_L_B<3,T,RGB<T>> {
		public:
			using EXT::H_L_B<3, T, RGB<T>>::H_L_B;

		public:
			T Illumination() const { return static_cast<T>(0.2126) * r + static_cast<T>(0.7152) * g + static_cast<T>(0.0722) * b; }
			T Gray() const { return r * static_cast<T>(0.299) + g * static_cast<T>(0.587) + b * static_cast<T>(0.114); }
		};
	}

	template<typename T>
	using RGB = Basic::RGB<T>;

	using RGBf = RGB<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_RGB_H_
