#ifndef _CPPUTIL_BASIC_MATH_VAL3_H_
#define _CPPUTIL_BASIC_MATH_VAL3_H_

#include <iostream>
#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Val3 {
		public:
			Val3() : x(0), y(0), z(0) { }
			explicit Val3(T val) : x(val), y(val), z(val) { }
			Val3(T x, T y, T z) : x(x), y(y), z(z) { }
			Val3(const Val3 & val3) : x(val3.x), y(val3.y), z(val3.z) { }
			template<typename U>
			Val3(const Val3<U> & val3) :
				x(staic_cast<T>(val3.x)),
				y(staic_cast<T>(val3.y)),
				z(staic_cast<T>(val3.z)) { }

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

		using Val3f = Val3<float>;
		using Val3i = Val3<int>;
	}
}

#endif // !_CPPUTIL_BASIC_MATH_VAL3_H_
