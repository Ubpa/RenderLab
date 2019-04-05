#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_BASIC_VAL2_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_BASIC_VAL2_H_

#include <CppUtil/Basic/UGM/Val2.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val.h>

#include <CppUtil/Basic/Math.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <typename T, typename ImplT>
			class Basic_Val_Base<2, T, ImplT> : public Val<2, T> {
			public:
				using Val<2, T>::Val;

			public:
				template<typename U>
				const ImplT ToVal(U v) const{
					const T T_v = static_cast<T>(v);
					return ImplT(
						Math::ToVal(x, T_v),
						Math::ToVal(y, T_v)
					);
				}

				template<typename U>
				ImplT & SelfToVal(U v) const {
					const T T_v = static_cast<T>(v);
					x = Math::ToVal(x, T_v);
					y = Math::ToVal(y, T_v);
					return *static_cast<ImplT*>(this);
				}

				const ImplT Abs() const {
					return ImplT(
						abs(x),
						abs(y)
					);
				}

				ImplT & AbsSelf() {
					x = abs(x);
					y = abs(y);
					return *static_cast<ImplT*>(this);
				}

				static const ImplT Lerp(const ImplT & s0, const ImplT & s1, T t) {
					return ImplT(
						Math::Lerp(s0.x, s1.x, t),
						Math::Lerp(s0.y, s1.y, t)
					);
				}

				static const ImplT Min(const ImplT & lhs, const ImplT & rhs) {
					return ImplT(
						std::min(lhs.x, rhs.x),
						std::min(lhs.y, rhs.y)
					);
				}

				static const ImplT Max(const ImplT & lhs, const ImplT & rhs) {
					return ImplT(
						std::max(lhs.x, rhs.x),
						std::max(lhs.y, rhs.y)
					);
				}

				const ImplT operator*(const ImplT & v) const {
					return ImplT(
						lhs.x * rhs.x,
						lhs.y * rhs.y
					);
				}

				ImplT & operator*=(const ImplT & v) {
					x *= v.x;
					y *= v.y;
					z *= v.z;

				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_BASIC_VAL2_H_
