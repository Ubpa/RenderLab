#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_BASIC_VAL1_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_BASIC_VAL1_H_

#include <CppUtil/Basic/UGM/Val1.h>
#include <CppUtil/Basic/UGM/ext/Basic_Val.h>

#include <CppUtil/Basic/Math.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <typename T, typename ImplT>
			class Basic_Val_Base<1, T, ImplT> : public Val<1, T> {
			public:
				using Val<1, T>::Val;

			public:
				template<typename U>
				const ImplT ToVal(U v) const{
					const T T_v = static_cast<T>(v);
					return ImplT(
						Math::ToVal(x, T_v)
					);
				}

				template<typename U>
				ImplT & SelfToVal(U v) const {
					const T T_v = static_cast<T>(v);
					x = Math::ToVal(x, T_v);
					return *static_cast<ImplT*>(this);
				}

				const ImplT Abs() const {
					return ImplT(
						abs(x)
					);
				}

				ImplT & AbsSelf() {
					x = abs(x);
					return *static_cast<ImplT*>(this);
				}

				static const ImplT Lerp(const ImplT & s0, const ImplT & s1, T t) {
					return ImplT(
						Math::Lerp(s0.x, s1.x, t)
					);
				}

				static const ImplT Min(const ImplT & lhs, const ImplT & rhs) {
					return ImplT(
						std::min(lhs.x, rhs.x)
					);
				}

				static const ImplT Max(const ImplT & lhs, const ImplT & rhs) {
					return ImplT(
						std::max(lhs.x, rhs.x)
					);
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_BASIC_VAL1_H_
