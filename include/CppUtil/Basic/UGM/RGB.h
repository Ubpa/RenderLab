#ifndef _CPPUTIL_BASIC_MATH_UGM_RGB_H_
#define _CPPUTIL_BASIC_MATH_UGM_RGB_H_

#include <CppUtil/Basic/UGM/ext/Basic_Val3.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class RGB : public EXT::Basic_Val<3,T,RGB<T>> {
		public:
			using EXT::Basic_Val<3, T, RGB<T>>::Basic_Val;

		public:
			T Illumination() const { return static_cast<T>(0.2126) * r + static_cast<T>(0.7152) * g + static_cast<T>(0.0722) * b; }
			T Gray() const { return r * static_cast<T>(0.299) + g * static_cast<T>(0.587) + b * static_cast<T>(0.114); }

		public:
			const RGB operator+(const RGB & rgb) const {
				return RGB(r + rgb.r, g + rgb.g, b + rgb.b);
			}

			RGB & operator+=(const RGB & rgb) {
				r += rgb.r;
				g += rgb.g;
				b += rgb.b;
				return *this;
			}

			const RGB operator+(T val)const {
				return RGB(r + val, g + val, b + val);
			}

			RGB operator+=(T val) {
				r += val;
				g += val;
				b += val;
				return *this;
			}

			const RGB operator-() const { return RGB(-r, -g, -b); }

			const RGB operator-(const RGB & rgb) const {
				return RGB(r - rgb.r, g - rgb.g, b - rgb.b);
			}

			RGB & operator-=(const RGB &rgb) {
				r -= rgb.r;
				g -= rgb.g;
				b -= rgb.b;
				return *this;
			}

			const RGB operator-(T val)const {
				return RGB(r - val, g - val, b - val);
			}

			RGB operator-=(T val) {
				r -= val;
				g -= val;
				b -= val;
				return *this;
			}
			
			const RGB operator*(T s) const {
				return RGB(s * r, s * g, s * b);
			}

			RGB & operator*=(T s) {
				r *= s;
				g *= s;
				b *= s;
				return *this;
			}

			const RGB operator*(const RGB & rhs) const {
				return RGB(r*rhs.r, g*rhs.g, b*rhs.b);
			}

			RGB & operator*=(const RGB & rhs) {
				r *= rhs.r;
				g *= rhs.g;
				b *= rhs.b;
				return *this;
			}

			template <typename U>
			const RGB operator/(U f) const {
				assert(f != static_cast<U>(0));
				const float inv = (float)1 / f;
				return RGB(r * inv, g * inv, b * inv);
			}

			template <typename U>
			RGB & operator/=(U f) {
				assert(f != static_cast<U>(0));
				const float inv = (float)1 / f;
				r *= inv;
				g *= inv;
				b *= inv;
				return *this;
			}
		};

		template<typename T, typename U>
		const RGB<T> operator+(U k, const RGB<T> & rgb) {
			return rgb + k;
		}

		template<typename T, typename U>
		const RGB<T> operator-(U k, const RGB<T> & rgb) {
			return -rgb + k;
		}

		template<typename T, typename U>
		const RGB<T> operator*(U k, const RGB<T> & rgb) {
			return rgb * k;
		}
	}

	template<typename T>
	using RGB = Basic::RGB<T>;

	using RGBf = RGB<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_RGB_H_
