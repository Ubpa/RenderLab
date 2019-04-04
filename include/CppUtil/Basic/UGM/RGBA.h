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

		public:
			const RGBA operator+(const RGBA &v) const {
				return RGBA(r + v.r, g + v.g, b + v.b, a + v.a);
			}

			RGBA & operator+=(const RGBA &v) {
				r += v.r;
				g += v.g;
				b += v.b;
				a += v.a;
				return *this;
			}

			const RGBA operator-(const RGBA &v) const {
				return RGBA(r - v.r, g - v.g, b - v.b, a - v.a);
			}

			const RGBA operator-() const { return RGBA(-r, -g, -b, -a); }

			RGBA & operator-=(const RGBA &v) {
				r -= v.r;
				g -= v.g;
				b -= v.b;
				a -= v.a;
				return *this;
			}

			template <typename U>
			const RGBA operator*(U s) const {
				return RGBA(s * r, s * g, s * b, s * a);
			}

			template <typename U>
			RGBA & operator*=(U s) {
				r *= s;
				g *= s;
				b *= s;
				a *= s;
				return *this;
			}

			template <typename U>
			const RGBA operator/(U f) const {
				assert(f != static_cast<U>(0));
				const float inv = (float)1 / f;
				return RGBA(r * inv, g * inv, b * inv, a * inv);
			}

			template <typename U>
			RGBA & operator/=(U f) {
				assert(f != static_cast<U>(0));
				const float inv = (float)1 / f;
				r *= inv;
				g *= inv;
				b *= inv;
				a *= inv;
				return *this;
			}
		};

		template<typename T, typename U>
		const RGBA<T> operator*(U k, const RGBA<T> & rgba) {
			return rgba * k;
		}
	}

	template<typename T>
	using RGBA = Basic::RGBA<T>;

	using RGBAf = RGBA<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_RGBA_H_
