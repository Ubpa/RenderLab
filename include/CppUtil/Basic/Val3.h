#ifndef _CPPUTIL_BASIC_MATH_VAL3_H_
#define _CPPUTIL_BASIC_MATH_VAL3_H_

#include <iostream>
#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Val2;

		template <typename T>
		class Val4;

		template <typename T>
		class Val3 {
		public:
			template<typename U, typename V, typename W>
			Val3(U x, V y, W z) :
				x(static_cast<T>(x)),
				y(static_cast<T>(y)),
				z(static_cast<T>(z)) { }

			Val3(T val) : Val3(val, val, val) { }

			Val3() : Val3(static_cast<T>(0)) { }

			template<typename U, typename V>
			Val3(const Val2<U> & xy, V z) : Val3(xy.x, xy.y, z) { }

			template<typename U, typename V>
			Val3(U x, const Val2<U> & yz) : Val3(x, yz.x, yz.y) { }

			template<typename U>
			Val3(const Val3<U> & xyz) : Val3(xyz.x, xyz.y, xyz.z) { }

			template<typename U>
			Val3(const Val4<U> & val4) : Val3(val4.x, val4.y, val4.z) { }

		public:
			bool HasNaN() const { return std::isnan(x) || std::isnan(y) || std::isnan(z); }

			const T * Data() const { return _data; }
			T * Data() { return _data; }

			T MinComponent() const {
				return std::min(x, std::min(y, z));
			}
			T MaxComponent() const {
				return std::max(x, std::max(y, z));
			}
			int MinDim() const {
				return (x < y) ? ((x < z) ? 0 : 2) : ((y < z) ? 1 : 2);
			}
			int MaxDim() const {
				return (x > y) ? ((x > z) ? 0 : 2) : ((y > z) ? 1 : 2);
			}

		public:
			T operator[](int i) const { return _data[i]; }
			T & operator[](int i) { return _data[i]; }

			bool operator==(const Val3<T> & rhs) const {
				return x == rhs.x && y == rhs.y && z == rhs.z;
			}
			bool operator!=(const Val3<T> & rhs) const {
				return x != rhs.x || y != rhs.y || z != rhs.z;
			}

			Val3 & operator=(const Val3 & rhs) {
				x = rhs.x;
				y = rhs.y;
				z = rhs.z;
				return *this;
			}

		public:
			union
			{
				T _data[3];
				struct {
					T x, y, z;
				};
			};
		};

		template <typename T>
		std::ostream & operator<<(std::ostream & os, const Val3<T> & val3) {
			os << "[" << val3.x << ", " << val3.y << ", " << val3.z << "]";
			return os;
		}
	}

	using Val3f = Basic::Val3<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_VAL3_H_
