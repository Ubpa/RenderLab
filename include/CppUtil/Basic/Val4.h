#ifndef _CPPUTIL_BASIC_MATH_VAL4_H_
#define _CPPUTIL_BASIC_MATH_VAL4_H_

#include <iostream>
#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Val4 {
		public:
			Val4() : x(0), y(0), z(0), w(0) { }
			explicit Val4(T val) : x(val), y(val), z(val), w(val) { }
			Val4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }
			Val4(const Val4 & val4) : x(val4.x), y(val4.y), z(val4.z), w(val4.w) { }
			template<typename U>
			Val4(const Val4<U> & val4) :
				x(staic_cast<T>(val4.x)),
				y(staic_cast<T>(val4.y)),
				z(staic_cast<T>(val4.z)),
				w(staic_cast<T>(val4.w)) { }

		public:
			bool HasNaN() const { return std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w); }

			const T * Data() const { return _data; }
			T * Data() { return _data; }

			T MinComponent() const {
				return std::min(x, std::min(y, std::min(z, w)));
			}
			T MaxComponent() const {
				return std::max(x, std::max(y, std::max(z, w)));
			}
			int MinDim() const {
				return x < y && x < z && x < w ? 0 : (y < z && y < w ? 1 : (z < w ? 2 : 3));
			}
			int MaxDim() const {
				return x > y && x > z && x > w ? 0 : (y > z && y > w ? 1 : (z > w ? 2 : 3));
			}

		public:
			T operator[](int i) const { return _data[i]; }
			T & operator[](int i) { return _data[i]; }

			bool operator==(const Val4<T> & rhs) const {
				return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
			}
			bool operator!=(const Val4<T> & rhs) const {
				return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
			}

			Val4 & operator=(const Val4 & rhs) {
				x = rhs.x;
				y = rhs.y;
				z = rhs.z;
				w = rhs.w;
				return *this;
			}

		public:
			union
			{
				T _data[4];
				struct {
					T x, y, z, w;
				};
			};
		};

		template <typename T>
		std::ostream & operator<<(std::ostream & os, const Val4<T> & val4) {
			os << "[" << val4.x << ", " << val4.y << ", " << val4.z << ", " << val4.w << "]";
			return os;
		}

		using Val4f = Val4<float>;
		using Val4i = Val4<int>;
	}
}

#endif // !_CPPUTIL_BASIC_MATH_VAL4_H_
