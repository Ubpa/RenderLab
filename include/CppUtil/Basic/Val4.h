#ifndef _CPPUTIL_BASIC_MATH_VAL4_H_
#define _CPPUTIL_BASIC_MATH_VAL4_H_

#include <iostream>
#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Val3;
		template <typename T>
		class Val2;

		template <typename T>
		class Val4 {
		public:
			template <typename T1, typename T2, typename T3, typename T4>
			Val4(T1 x, T2 y, T3 z, T4 w) :
				x(static_cast<T>(x)),
				y(static_cast<T>(y)),
				z(static_cast<T>(z)),
				w(static_cast<T>(w)) { }

			template<typename U>
			explicit Val4(U val) : Val4(val, val, val, val) { }

			Val4() : Val4(0) { }

			template<typename U, typename V, typename W>
			Val4(const Val2<U> & xy, V z, W w) : Val4(xy.x, xy.y, z, w) { }

			template<typename U, typename V, typename W>
			Val4(U x, const Val2<V> & yz, W w) : Val4(x, yz.x, yz.y, w) { }

			template<typename U, typename V, typename W>
			Val4(U x, V y, const Val2<W> & zw) : Val4(x, y, zw.x, zw.y) { }

			template<typename U, typename V>
			Val4(const Val2<U> & xy, const Val2<V> & zw) : Val4(xy.x, xy.y, zw.x, zw.y) { }

			template<typename U, typename V>
			Val4(const Val3<U> & xyz, V w) : Val4(xyz.x, xyz.y, xyz.z, w) { }

			template<typename U, typename V>
			Val4(U x, const Val3<V> & yzw) : Val4(x, yzw.x, yzw.y, yzw.z) { }

			template<typename U>
			Val4(const Val4<U> & val4) : Val4(val4.x, val4.y, val4.z, val4.w) { }

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
