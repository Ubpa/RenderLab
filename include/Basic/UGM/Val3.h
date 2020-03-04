#ifndef _CPPUTIL_BASIC_MATH_UGM_VAL3_H_
#define _CPPUTIL_BASIC_MATH_UGM_VAL3_H_

#include <Basic/UGM/Val.h>

#include <algorithm>

namespace CppUtil {
	namespace Basic {

		template <typename T>
		class Val<3, T> {
		public:
			static constexpr int valNum = 3;
			using valType = T;

		public:
			template<typename U, typename V, typename W>
			Val(U x, V y, W z) :
				x(static_cast<T>(x)),
				y(static_cast<T>(y)),
				z(static_cast<T>(z)) {
				assert(!HasNaN());
			}

			template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
			Val(U val) : Val(val, val, val) { }

			Val() : Val(0) { }

			template<typename U, int N, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
			Val(U(&arr)[N]) : Val(arr[0], arr[1], arr[2]) { assert(N >= valNum); }

			template<typename U, typename V>
			Val(const Val<2, U> & xy, V z) : Val(xy.x, xy.y, z) { }

			template<typename U, typename V>
			Val(U x, const Val<2, U> & yz) : Val(x, yz.x, yz.y) { }

			template<typename U>
			Val(const Val<3, U> & xyz) : Val(xyz.x, xyz.y, xyz.z) { }

			template<typename U>
			Val(const Val<4, U> & val4) : Val(val4.x, val4.y, val4.z) { }

		public:
			bool HasNaN() const { return std::isnan<double>(x) || std::isnan<double>(y) || std::isnan<double>(z); }
			bool IsVal(T val) const {
				return Math::ToVal(x, val) == val && Math::ToVal(y, val) == val && Math::ToVal(z, val) == val;
			}
			bool IsZero() const {
				return IsVal(static_cast<T>(0));
			}

			const T * Data() const & { return _data; }
			T * Data() & { return _data; }

			T & MinComponent() {
				return _data[MinDim()];
			}
			const T & MinComponent() const {
				return std::min(x, std::min(y, z));
			}
			T & MaxComponent() {
				return _data[MaxDim()];
			}
			const T & MaxComponent() const {
				return std::max(x, std::max(y, z));
			}
			int MinDim() const {
				return (x < y) ? ((x < z) ? 0 : 2) : ((y < z) ? 1 : 2);
			}
			int MaxDim() const {
				return (x > y) ? ((x > z) ? 0 : 2) : ((y > z) ? 1 : 2);
			}

		public:
			const T & operator[](int i) const { assert(i >= 0 && i < valNum); return _data[i]; }
			T & operator[](int i) { assert(i >= 0 && i < valNum); return _data[i]; }

			bool operator==(const Val & rhs) const {
				return Math::Equal(x, rhs.x) && Math::Equal(y, rhs.y) && Math::Equal(z, rhs.z);
			}

			bool operator!=(const Val & rhs) const {
				return !Math::Equal(x, rhs.x) || !Math::Equal(y, rhs.y) || !Math::Equal(z, rhs.z);
			}

			Val & operator=(const Val & rhs) {
				x = rhs.x;
				y = rhs.y;
				z = rhs.z;
				return *this;
			}

			bool operator<(const Val & v) const {
				return
					x < v.x || x == v.x && (
						y < v.y || y == v.y && (
							z < v.z ));
			}

			bool operator<=(const Val & v) const {
				return
					x < v.x || x == v.x && (
						y < v.y || y == v.y && (
							z <= v.z));
			}

		public:
			union
			{
				T _data[3];
				struct {
					union { T x, r, s; };
					union { T y, g, t; };
					union { T z, b, p; };
				};
			};
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VAL3_H_
