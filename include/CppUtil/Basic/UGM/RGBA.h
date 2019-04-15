#ifndef _CPPUTIL_BASIC_MATH_UGM_RGBA_H_
#define _CPPUTIL_BASIC_MATH_UGM_RGBA_H_

#include <CppUtil/Basic/UGM/RGB.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val4.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class RGBA : public EXT::Basic_Val<4,T,RGBA<T>> {
		public:
			using EXT::Basic_Val<4, T, RGBA<T>>::Basic_Val;

		public:
			explicit RGBA(T val) : RGBA(val, val, val, 1) { }

			RGBA() : RGBA(static_cast<T>(0)) { }

			template<typename U, typename V>
			RGBA(const Val<2, U> & rg, V b) : RGBA(rg, b, 1) { }

			template<typename U, typename V>
			RGBA(U r, const Val<2, V> & gb) : RGBA(r, gb, 1) { }

			template<typename U>
			RGBA(const Val<3, U> & rbg) : RGBA(rbg, 1) { }

			template<typename U, typename V>
			RGBA(const RGB<U> & rgb, V a) : RGBA(rgb.r, rgb.g, rgb.b, a) { }

			template<typename U, typename V>
			RGBA(const RGB<U> & rgb) : RGBA(rgb.r, rgb.g, rgb.b, 1) { }

			template<typename U>
			RGBA(const RGBA<U> & rgba) : RGBA(rgba.r, rgba.g, rgba.b, rgba.a) { }

		public:
			const RGB<T> ToRGB() const {
				return RGB<T>(r, g, b)*a;
			}
			const RGB<T> Over(const RGB<T> & rgb) const {
				return ToRGB() + rgb * (static_cast<T>(1) - a);
			}

			const RGBA Over(const RGBA & rgba) const {
				// rstA = 1 - (1-a)(1-rgba.a)
				const auto rstA = a + rgba.a - a * rgba.a;
				if (rstA == static_cast<T>(0))
					return RGBA(0, 0, 0, 0);

				const auto rstRGB = (ToRGB() + rgba.ToRGB()*a) / rstA;
				return RGBA(rstRGB, rstA);
			}

#ifndef NDEBUG
			// 仅用于 Debug 时方便在 IDE 中显示结果
		public:
			RGBA & operator =(const RGBA & rgba) {
				*static_cast<EXT::Basic_Val<4, T, RGBA<T>>*>(this) = rgba;
				return *this;
			}
		private:
			const T & _r{ *(((T*)this) + 0) };
			const T & _g{ *(((T*)this) + 1) };
			const T & _b{ *(((T*)this) + 2) };
			const T & _a{ *(((T*)this) + 3) };
#endif // !NDEBUG
		};

	}

	template<typename T>
	using RGBA = Basic::RGBA<T>;

	using RGBAf = RGBA<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_RGBA_H_
