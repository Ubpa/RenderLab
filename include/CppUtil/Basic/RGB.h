#ifndef _CPPUTIL_BASIC_MATH_RGB_H_
#define _CPPUTIL_BASIC_MATH_RGB_H_

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
		class RGB {
		public:
			template<typename U, typename V, typename W>
			RGB(U r, V g, W b) :
				r(static_cast<T>(r)),
				g(static_cast<T>(g)),
				b(static_cast<T>(b)) { }

			explicit RGB(T val) : RGB(val, val, val) { }

			RGB() : RGB(static_cast<T>(0)) { }

			template<typename U, typename V>
			RGB(const Val2<U> & val2, V b) : RGB(val2.x, val2.y, b) { }

			template<typename U, typename V>
			RGB(U r, const Val2<V> & val2) : RGB(r, val2.x, val2.y) { }

			template<typename U>
			RGB(const Val3<U> & val3) : RGB(val3.x, val3.y, val3.z) { }

			template<typename U>
			RGB(const Val4<U> & val4) : RGB(val4.x, val4.y, val4.z) { }


		public:
			T operator[](int i)const { return _data[i]; }
			T & operator[](int i) { return _data[i]; }

		public:
			bool HasNaN() const { return isnan(r) || isnan(g) || isnan(b); }

			T Illumination() const { return static_cast<T>(0.2126) *r + static_cast<T>(0.7152) * g + static_cast<T>(0.0722) * b; }

			static const RGB Lerp(const RGB & c0, const RGB & c1, float t) {
				return (1.f - t)*c0 + t * c1;
			}
			const RGB LerpWith(const RGB & c1, float t) const {
				return Lerp(*this, c1, t);
			}

		public:
			const RGB operator+(const RGB &v) const {
				return RGB(r + v.r, g + v.g, b + v.b);
			}

			RGB & operator+=(const RGB &v) {
				r += v.r;
				g += v.g;
				b += v.b;
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

			const RGB operator-(const RGB &v) const {
				return RGB(r - v.r, g - v.g, b - v.b);
			}

			RGB & operator-=(const RGB &v) {
				r -= v.r;
				g -= v.g;
				b -= v.b;
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

			template <typename U>
			const RGB operator*(U s) const {
				return RGB(s * r, s * g, s * b);
			}

			template <typename U>
			RGB & operator*=(U s) {
				r *= s;
				g *= s;
				b *= s;
				return *this;
			}

			const RGB operator*(const RGB & rhs) const {
				return RGB(r*rhs.r, g*rhs.g, b*rhs.b);
			}

			RGB & operator*(const RGB & rhs) {
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

		public:
			union
			{
				T _data[3];
				struct
				{
					T r, g, b;
				};
			};
		};

		template<typename T>
		std::ostream & operator<<(std::ostream & os, const RGB<T> & rgb) {
			os << "[" << rgb.r << ", " << rgb.g << ", " << rgb.b << "]";
			return os;
		}

		template<typename U, typename V>
		const RGB<U> operator*(V k, const RGB<U> & rgb) {
			return rgb * k;
		}

		template<typename U, typename V>
		const RGB<U> operator+(V k, const RGB<U> & rgb) {
			return rgb + k;
		}

		template<typename U, typename V>
		const RGB<U> operator-(V k, const RGB<U> & rgb) {
			return rgb - k;
		}
	}

	using RGBf = Basic::RGB<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_RGB_H_
