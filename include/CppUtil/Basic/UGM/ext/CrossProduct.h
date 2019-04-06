#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_CROSS_PRODUCT_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_CROSS_PRODUCT_H_

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			// 要求 BaseT 是 3 维的
			template<typename T, typename BaseT, typename ImplT>
			class CrossProduct : public BaseT {
			public:
				using BaseT::BaseT;

			public:
				static const ImplT Cross(const ImplT & lhs, const ImplT & rhs) {
					//  i  j  k
					// x0 y0 z0
					// x1 y1 z1
					return ImplT(
						lhs.y*rhs.z - lhs.z*rhs.y,
						lhs.z*rhs.x - lhs.x*rhs.z,
						lhs.x*rhs.y - lhs.y*rhs.x
					);
				}

				const ImplT Cross(const ImplT & v) const {
					return Cross(*static_cast<const ImplT *>(this), v);
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_CROSS_PRODUCT_H_
