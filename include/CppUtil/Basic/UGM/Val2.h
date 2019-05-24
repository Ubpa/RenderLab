#ifndef _CPPUTIL_BASIC_MATH_UGM_VAL2_H_
#define _CPPUTIL_BASIC_MATH_UGM_VAL2_H_

#include <CppUtil/Basic/UGM/Val.h>

#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Val<2, T> {
		public:
			static constexpr int valNum = 2;
			using valType = T;

		public:
			template<typename U, typename V>
			Val(U x, V y) :
				x(static_cast<T>(x)),
				y(static_cast<T>(y)) {
				assert(!HasNaN());
			}

			explicit Val(T val) : Val(val, val) { }

			Val() : Val(static_cast<T>(0)) { }

			template<typename U, int N>
			Val(U(&arr)[N]) : Val(arr[0], arr[1]) { assert(N >= 2); }

			template<typename U>
			explicit Val(const Val<2, U> & xy) : Val(xy.x, xy.y) { }

			template<typename U>
			explicit Val(const Val<3, U> & val3) : Val(val3.x, val3.y) { }

			template<typename U>
			explicit Val(const Val<4, U> & val4) : Val(val4.x, val4.y) { }

		public:
			bool HasNaN() const { return std::isnan<double>(x) || std::isnan<double>(y); }
			const bool IsVal(T val) const {
				return Math::ToVal(x, val) == val && Math::ToVal(y, val) == val;
			}
			const bool IsZero() const {
				return IsVal(static_cast<T>(0));
			}

			const T * Data() const & { return _data; }
			T * Data() & { return _data; }

			const T & MinComponent() const {
				return std::min(x, y);
			}
			const T & MaxComponent() const {
				return std::max(x, y);
			}
			const int MinDim() const {
				return x < y ? 0 : 1;
			}
			const int MaxDim() const {
				return x > y ? 0 : 1;
			}

		public:
			const T & operator[](int i) const { assert(i >= 0 && i < valNum); return _data[i]; }
			T & operator[](int i) { assert(i >= 0 && i < valNum); return _data[i]; }

			bool operator==(const Val & rhs) const {
				return Math::Equal(x, rhs.x) && Math::Equal(y, rhs.y);
			}

			bool operator!=(const Val & rhs) const {
				return !Math::Equal(x, rhs.x) || !Math::Equal(y, rhs.y);
			}

			Val & operator=(const Val & rhs) {
				x = rhs.x;
				y = rhs.y;
				return *this;
			}

		public:
			union
			{
				T _data[2];
				struct {
					union { T x, r, s; };
					union { T y, g, t; };
				};
			};
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VAL2_H_
