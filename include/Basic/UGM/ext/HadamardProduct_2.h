#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_HADAMARD_PRODUCT_2_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_HADAMARD_PRODUCT_2_H_

#include <Basic/UGM/ext/HadamardProduct.h>

#include <cassert>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template<typename T, typename BaseT, typename ImplT>
			class HadamardProduct_Base<2, T, BaseT, ImplT> : public BaseT {
			public:
				using BaseT::BaseT;

			public:
				using BaseT::operator*;
				using BaseT::operator*=;
				using BaseT::operator/;
				using BaseT::operator/=;

				const ImplT operator*(const ImplT & v) const {
					return ImplT(
						x * v.x,
						y * v.y
					);
				}

				ImplT & operator*=(const ImplT & v) {
					x *= v.x;
					y *= v.y;
					return *static_cast<ImplT*>(this);
				}

				const ImplT operator/(const ImplT & v) const {
					assert(v.x != static_cast<T>(0));
					assert(v.y != static_cast<T>(0));

					return ImplT(
						x / v.x,
						y / v.y
					);
				}

				ImplT & operator/=(const ImplT & v) {
					assert(v.x != static_cast<T>(0));
					assert(v.y != static_cast<T>(0));

					x /= v.x;
					y /= v.y;

					return *static_cast<ImplT*>(this);
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_HADAMARD_PRODUCT_2_H_
