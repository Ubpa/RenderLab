#ifndef _CPPUTIL_BASIC_MATH_VAL2_H_
#define _CPPUTIL_BASIC_MATH_VAL2_H_

#include <iostream>
#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Val3;
		template<typename T>
		class Val4;

		template <typename T>
		class Val2 {
		public:
			template<typename U, typename V>
			Val2(U x, V y) :
				x(static_cast<T>(x)),
				y(static_cast<T>(y)) { }

			template<typename U>
			explicit Val2(U val) : Val2(val, val) { }

			Val2() : Val2(0) { }

			template<typename U>
			Val2(const Val2<U> & xy) : Val2(xy.x, xy.y) { }

			template<typename U>
			Val2(const Val3<U> & val3) : Val2(val3.x, val3.y) { }

			template<typename U>
			Val2(const Val4<U> & val4) : Val2(val4.x, val4.y) { }

		public:
			bool HasNaN() const { return std::isnan(x) || std::isnan(y) || std::isnan(z); }

			const T * Data() const { return _data; }
			T * Data() { return _data; }

			T MinComponent() const {
				return std::min(x, y);
			}
			T MaxComponent() const {
				return std::max(x, y);
			}
			int MinDim() const {
				return x < y ? 0 : 1;
			}
			int MaxDim() const {
				return x > y ? 0 : 1;
			}

		public:
			T operator[](int i) const { return _data[i]; }
			T & operator[](int i) { return _data[i]; }

			bool operator==(const Val2<T> & rhs) const {
				return x == rhs.x && y == rhs.y;
			}
			bool operator!=(const Val2<T> & rhs) const {
				return x != rhs.x || y != rhs.y;
			}

			Val2 & operator=(const Val2 & rhs) {
				x = rhs.x;
				y = rhs.y;
				return *this;
			}

		public:
			union
			{
				T _data[2];
				struct {
					T x, y;
				};
			};
		};

		template <typename T>
		std::ostream & operator<<(std::ostream & os, const Val2<T> & val2) {
			os << "[" << val2.x << ", " << val2.y << "]";
			return os;
		}

		using Val2f = Val2<float>;
		using Val2i = Val2<int>;
	}
}

#endif // !_CPPUTIL_BASIC_MATH_VAL2_H_
