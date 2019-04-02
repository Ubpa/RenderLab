#ifndef _CPPUTIL_BASIC_MATH_RGB_H_
#define _CPPUTIL_BASIC_MATH_RGB_H_

#include <CppUtil/Basic/Val3.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class RGB : public Val<3, T> {
		public:
			using Val<3, T>::Val;

		public:
			T Illumination() const { return static_cast<T>(0.2126) * r + static_cast<T>(0.7152) * g + static_cast<T>(0.0722) * b; }
			T Gray() const { return r * static_cast<T>(0.299) + g * static_cast<T>(0.587) + b * static_cast<T>(0.114); }

			static const RGB Lerp(const RGB & c0, const RGB & c1, float t) {
				return (1.f - t)*c0 + t * c1;
			}
			const RGB LerpWith(const RGB & c1, float t) const {
				return Lerp(*this, c1, t);
			}

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
				const float inv = (float)1 / f;
				return RGB(r * inv, g * inv, b * inv);
			}

			template <typename U>
			RGB & operator/=(U f) {
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

#endif // !_CPPUTIL_BASIC_MATH_RGB_H_
