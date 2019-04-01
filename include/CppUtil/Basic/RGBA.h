#ifndef _CPPUTIL_BASIC_MATH_RGBA_H_
#define _CPPUTIL_BASIC_MATH_RGBA_H_

#include <CppUtil/Basic/RGB.h>
#include <iostream>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Val2;
		template<typename T>
		class Val3;
		template<typename T>
		class Val4;

		template<typename T>
		class RGBA {
		public:
			template<typename T1, typename T2, typename T3, typename T4>
			RGBA(T1 r, T2 g, T3 b, T4) :
				r(static_cast<T>(r)),
				g(static_cast<T>(g)),
				b(static_cast<T>(b)),
				a(static_cast<T>(a)){ }

			explicit RGBA(T val) : RGBA(val, val, val, 1) { }

			RGBA() : RGBA(static_cast<T>(0)) { }

			template<typename U, typename V, typename W>
			RGBA(const Val2<U> & rg, V b, W a) : RGBA(rg.x, rg.y, b, a) { }

			template<typename U, typename V, typename W>
			RGBA(U r, const Val2<V> & gb, W a) : RGBA(r, gb.x, gb.y, a) { }

			template<typename U, typename V, typename W>
			RGBA(U r, V g, const Val2<W> & ba) : RGBA(r, g, ba.x, ba.y) { }

			template<typename U, typename V>
			RGBA(const Val2<U> & rg, V b) : RGBA(rg, b, 1) { }

			template<typename U, typename V>
			RGBA(U r, const Val2<V> & gb) : RGBA(r, gb, 1) { }

			template<typename U, typename V>
			RGBA(const Val3<U> & rgb, V a) : RGBA(rgb.x, rgb.y, rgb.z, a) { }

			template<typename U>
			RGBA(const Val3<U> & rbg) : RGBA(rbg, 1) { }

			template<typename U, typename V>
			RGBA(const RGB<U> & rgb, V a) : RGBA(rgb.r, rgb.g, rgb.b, a) { }

			template<typename U, typename V>
			RGBA(const RGB<U> & rgb) : RGBA(rgb.r, rgb.g, rgb.b, 1) { }

			template<typename U>
			RGBA(const Val4<U> & rgba) : RGBA(rgba.x, rgba.y, rgba.z, rgba.w) { }

			template<typename U>
			RGBA(const RGBA<U> & rgba) : RGBA(rgba.r, rgba.g, rgba.b, rgba.a) { }

		public:
			T operator[](int i)const { return _data[i]; }
			T & operator[](int i) { return _data[i]; }

		public:
			bool HasNaN() const { return isnan(r) || isnan(g) || isnan(b) || isnan(a); }

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

		public:
			union
			{
				T _data[4];
				struct
				{
					T r, g, b, a;
				};
			};
		};

		template<typename T>
		std::ostream & operator<<(std::ostream & os, const RGBA<T> & rgba) {
			os << "[" << rgba.r << ", " << rgba.g << ", " << rgba.b << ", " << rgba.a << "]";
			return os;
		}

		template<typename U, typename V>
		const RGBA<U> operator*(V k, const RGBA<U> & rgba) {
			return rgba * k;
		}
	}

	using RGBAf = Basic::RGBA<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_RGBA_H_
