#ifndef _CPPUTIL_BASIC_MATH_VAL4_H_
#define _CPPUTIL_BASIC_MATH_VAL4_H_

#include <CppUtil/Basic/Val.h>
#include <CppUtil/Basic/Vector3.h>

#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Val<4, T> {
		public:
			template <typename T1, typename T2, typename T3, typename T4>
			Val(T1 x, T2 y, T3 z, T4 w) :
				x(static_cast<T>(x)),
				y(static_cast<T>(y)),
				z(static_cast<T>(z)),
				w(static_cast<T>(w)) { }

			explicit Val(T val) : Val(val, val, val, val) { }

			Val() : Val(static_cast<T>(0)) { }

			template<typename U, typename V, typename W>
			Val(const Val<2, U> & xy, V z, W w) : Val(xy.x, xy.y, z, w) { }

			template<typename U, typename V, typename W>
			Val(U x, const Val<2, V> & yz, W w) : Val(x, yz.x, yz.y, w) { }

			template<typename U, typename V, typename W>
			Val(U x, V y, const Val<2, W> & zw) : Val(x, y, zw.x, zw.y) { }

			template<typename U, typename V>
			Val(const Val<2, U> & xy, const Val<2, V> & zw) : Val(xy.x, xy.y, zw.x, zw.y) { }

			template<typename U, typename V>
			Val(const Val<3, U> & xyz, V w) : Val(xyz.x, xyz.y, xyz.z, w) { }

			template<typename U, typename V>
			Val(U x, const Val<3, V> & yzw) : Val(x, yzw.x, yzw.y, yzw.z) { }

			template<typename U>
			Val(const Val<4, U> & val4) : Val(val4.x, val4.y, val4.z, val4.w) { }

		public:
			bool HasNaN() const { return std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w); }
			bool IsZero() const {
				static constexpr T zero = static_cast<T>(0);
				return x == zero && y == zero && z == zero && w == zero;
			}

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

			bool operator==(const Val & rhs) const {
				return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
			}
			bool operator!=(const Val & rhs) const {
				return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
			}

			Val & operator=(const Val & rhs) {
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
					union { T x, r, s; };
					union { T y, g, t; };
					union { T z, b, p; };
					union { T w, a, q; };
				};
				struct {
					Vector<3, T> imag;
					T real;
				};
			};
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_VAL4_H_
