#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_2_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_2_H_

#include <CppUtil/Basic/UGM/ext/Linearity.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <typename T, typename BaseT, typename ImplT>
			class Linearity_Base<2, T, BaseT, ImplT> : public BaseT {
			public:
				using BaseT::BaseT;

			public:
				const ImplT operator+(const ImplT & v) const {
					return ImplT(
						x + v.x,
						y + v.y
					);
				}

				ImplT & operator+=(const ImplT & v) {
					x += v.x;
					y += v.y;
					return *static_cast<ImplT*>(this);
				}

				const ImplT operator-() const {
					return ImplT(
						-x,
						-y
					);
				}

				ImplT & NegativeSelf() const {
					x = -x;
					y = -y;
					return *static_cast<ImplT*>(this);
				}

				const ImplT operator-(const ImplT & v) const {
					return ImplT(
						x - v.x,
						y - v.y
					);
				}

				ImplT & operator-=(const ImplT & v) {
					x -= v.x;
					y -= v.y;
					return *static_cast<ImplT*>(this);
				}

				const ImplT operator*(T k) const {
					return ImplT(
						x * k,
						y * k
					);
				}

				ImplT & operator*=(T k) {
					x *= k;
					y *= k;
					return *static_cast<ImplT*>(this);
				}

				const ImplT operator/(T k) const {
					assert(static_cast<float>(k) != 0.f);
					const float inv = 1.f / static_cast<float>(k);
					return ImplT(
						x * inv,
						y * inv
					);
				}

				ImplT & operator/=(T k) {
					assert(static_cast<float>(k) != 0.f);
					const float inv = 1.f / static_cast<float>(k);
					x *= inv;
					y *= inv;
					return *static_cast<ImplT*>(this);
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_2_H_
