#ifndef _CPPUTIL_BASIC_MATH_RGBA_H_
#define _CPPUTIL_BASIC_MATH_RGBA_H_

#include <CppUtil/Basic/Val4.h>
#include <CppUtil/Basic/RGB.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class RGBA : public Val<4, T> {
		public:
			template<typename T1, typename T2, typename T3, typename T4>
			RGBA(T1 r, T2 g, T3 b, T4 a) : Val<4, T>(r, g, b, a) { }

			explicit RGBA(T val) : RGBA(val, val, val, 1) { }

			RGBA() : RGBA(static_cast<T>(0)) { }

			template<typename U, typename V, typename W>
			RGBA(const Val<2, U> & rg, V b, W a) : RGBA(rg.x, rg.y, b, a) { }

			template<typename U, typename V, typename W>
			RGBA(U r, const Val<2, V> & gb, W a) : RGBA(r, gb.x, gb.y, a) { }

			template<typename U, typename V, typename W>
			RGBA(U r, V g, const Val<2, W> & ba) : RGBA(r, g, ba.x, ba.y) { }

			template<typename U, typename V>
			RGBA(const Val<2, U> & rg, const Val<2, V> & ba) : RGBA(rg.x, rg.y, ba.x, ba.y) { }

			template<typename U, typename V>
			RGBA(const Val<2, U> & rg, V b) : RGBA(rg, b, 1) { }

			template<typename U, typename V>
			RGBA(U r, const Val<2, V> & gb) : RGBA(r, gb, 1) { }

			template<typename U, typename V>
			RGBA(const Val<3, U> & rgb, V a) : RGBA(rgb.x, rgb.y, rgb.z, a) { }

			template<typename U, typename V>
			RGBA(U r, const Val<3, V> & gba) : RGBA(r, gba.x, gba.y, gba.z) { }

			template<typename U>
			RGBA(const Val<3, U> & rbg) : RGBA(rbg, 1) { }

			template<typename U, typename V>
			RGBA(const RGB<U> & rgb, V a) : RGBA(rgb.r, rgb.g, rgb.b, a) { }

			template<typename U, typename V>
			RGBA(const RGB<U> & rgb) : RGBA(rgb.r, rgb.g, rgb.b, 1) { }

			template<typename U>
			RGBA(const Val<4, U> & rgba) : RGBA(rgba.x, rgba.y, rgba.z, rgba.w) { }

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
				const float inv = (float)1 / f;
				return RGBA(r * inv, g * inv, b * inv, a * inv);
			}

			template <typename U>
			RGBA & operator/=(U f) {
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

#endif // !_CPPUTIL_BASIC_MATH_RGBA_H_
