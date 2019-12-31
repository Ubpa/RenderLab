#ifndef _CPPUTIL_BASIC_MATH_UGM_VAL1_H_
#define _CPPUTIL_BASIC_MATH_UGM_VAL1_H_

#include <CppUtil/Basic/UGM/Val.h>

#include <algorithm>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Val<1, T> {
		public:
			static constexpr int valNum = 1;
			using valType = T;

		public:
			template<typename U, typename = std::enable_if_t<std::is_convertible_v<U,T>>>
			Val(U x) : x(static_cast<T>(x)) { assert(!HasNaN()); }

			Val() : Val(0) { }

			template<typename U, int N, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
			Val(U(&arr)[N]) : Val(arr[0]) { assert(N >= 1); }

			template<int N, typename U>
			explicit Val(const Val<N, U> & val) : Val(val.x) { }

		public:
			bool HasNaN() const { return std::isnan<double>(x); }
			const bool IsVal(T val) const {
				return Math::ToVal(x, val) == val;
			}
			const bool IsZero() const {
				return IsVal(static_cast<T>(0));
			}

			const T * Data() const & { return _data; }
			T * Data() & { return _data; }

			const T & MinComponent() const {
				return x;
			}
			const T & MaxComponent() const {
				return x;
			}
			constexpr int MinDim() const {
				return 0;
			}
			constexpr int MaxDim() const {
				return 0;
			}

		public:
			const T & operator[](int i) const { assert(i >= 0 && i < valNum); return _data[i]; }
			T & operator[](int i) { assert(i >= 0 && i < valNum); return _data[i]; }

			bool operator==(const Val & rhs) const {
				return Math::Equal(x, rhs.x);
			}

			bool operator!=(const Val & rhs) const {
				return !Math::Equal(x, rhs.x);
			}

			Val & operator=(const Val & rhs) {
				x = rhs.x;
				return *this;
			}

			bool operator<(const T & v) const {
				return
					x < v.x;
			}

			bool operator<=(const T & v) const {
				return
					x <= v.x;
			}

		public:
			union
			{
				T _data[1];
				struct {
					union { T x, r, s; };
				};
			};
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_VAL1_H_
