#ifndef _CPPUTIL_BASIC_MATH_UGM_VAL4_H_
#define _CPPUTIL_BASIC_MATH_UGM_VAL4_H_

#include <CppUtil/Basic/UGM/Val.h>
#include <CppUtil/Basic/UGM/Vector3.h>

#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Val<4, T> {
		public:
			static constexpr int valNum = 4;
			using valType = T;

		public:
			template <typename T1, typename T2, typename T3, typename T4>
			Val(T1 x, T2 y, T3 z, T4 w) :
				x(static_cast<T>(x)),
				y(static_cast<T>(y)),
				z(static_cast<T>(z)),
				w(static_cast<T>(w)) {
				assert(!HasNaN());
			}

			template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
			Val(U val) : Val(val, val, val, val) { }

			Val() : Val(0) { }

			template<typename U, int N, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
			Val(U(&arr)[N]) : Val(arr[0], arr[1], arr[2], arr[3]) { assert(N >= 4); }

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
			explicit Val(const Val<4, U> & val4) : Val(val4.x, val4.y, val4.z, val4.w) { }

		public:
			bool HasNaN() const { return std::isnan<double>(x) || std::isnan<double>(y) || std::isnan<double>(z) || std::isnan<double>(w); }
			bool IsVal(T val) const {
				return Math::ToVal(x, val) == val && Math::ToVal(y, val) == val && Math::ToVal(z, val) == val && Math::ToVal(w, val) == val;
			}
			bool IsZero() const {
				return IsVal(static_cast<T>(0));
			}

			const T * Data() const & { return _data; }
			T * Data() & { return _data; }

			constexpr const T & MinComponent() const {
				return std::min(x, std::min(y, std::min(z, w)));
			}
			constexpr const T & MaxComponent() const {
				return std::max(x, std::max(y, std::max(z, w)));
			}
			int MinDim() const {
				return x < y && x < z && x < w ? 0 : (y < z && y < w ? 1 : (z < w ? 2 : 3));
			}
			int MaxDim() const {
				return x > y && x > z && x > w ? 0 : (y > z && y > w ? 1 : (z > w ? 2 : 3));
			}

		public:
			const T & operator[](int i) const { assert(i >= 0 && i < valNum); return _data[i]; }
			T & operator[](int i) { assert(i >= 0 && i < valNum); return _data[i]; }

			bool operator==(const Val & rhs) const {
				return Math::Equal(x, rhs.x) && Math::Equal(y, rhs.y) && Math::Equal(z, rhs.z) && Math::Equal(w, rhs.w);
			}

			bool operator!=(const Val & rhs) const {
				return !Math::Equal(x, rhs.x) || !Math::Equal(y, rhs.y) || !Math::Equal(z, rhs.z) || !Math::Equal(w, rhs.w);
			}

			Val & operator=(const Val & rhs) {
				x = rhs.x;
				y = rhs.y;
				z = rhs.z;
				w = rhs.w;
				return *this;
			}

			bool operator<(const T & v) const {
				return
					x < v.x || x == v.x && (
						y < v.y || y == v.y && (
							z < v.z || z == v.z && (
								w < v.w)));
			}

			bool operator<=(const T & v) const {
				return
					x < v.x || x == v.x && (
						y < v.y || y == v.y && (
							z < v.z || z == v.z && (
								w <= v.w)));
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

#endif // !_CPPUTIL_BASIC_MATH_UGM_VAL4_H_
