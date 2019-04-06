#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_ALIAS_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_ALIAS_H_

#include <CppUtil/Basic/UGM/ext/Basic_Val.h>
#include <CppUtil/Basic/UGM/ext/Linearity.h>
#include <CppUtil/Basic/UGM/ext/InnerProduct.h>
#include <CppUtil/Basic/UGM/ext/CrossProduct.h>
#include <CppUtil/Basic/UGM/ext/Normed.h>
#include <CppUtil/Basic/UGM/ext/Metric.h>
#include <CppUtil/Basic/UGM/ext/HadamardProduct.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template<int N, typename T, MetricType metricT, typename ImplT>
			using M_B = Metric<N, T, metricT, Basic_Val<N, T, ImplT>, ImplT>;

			template<int N, typename T, typename ImplT>
			using ME_B = M_B<N, T, MetricType::Euclidean, ImplT>;

			template<int N, typename T, typename ImplT>
			using L_B = Linearity<N, T, Basic_Val<N, T, ImplT>, ImplT>;

			template<int N, typename T, typename ImplT>
			using H_L_B = HadamardProduct<N, T, L_B<N, T, ImplT>, ImplT>;

			template<int N, typename T, typename BaseT, typename ImplT>
			using IE = InnerProduct<N, T, InnerProductType::Euclidean, BaseT, ImplT>;

			template<int N, typename T, typename ImplT>
			using IE_L_B = IE<N, T, L_B<N, T, ImplT>, ImplT>;

			template<int N, typename T, typename BaseT, typename ImplT>
			using NI = Normed<N, T, NormType::InnerProduct, BaseT, ImplT>;

			template<int N, typename T, typename ImplT>
			using NI_IE_L_B = NI<N, T, IE_L_B<N, T, ImplT>, ImplT>;

			template<typename T, typename ImplT>
			using C_IE_L_B_3 = CrossProduct<T, IE_L_B<3, T, ImplT>, ImplT>;

			template<typename T, typename ImplT>
			using NI_C_IE_L_B_3 = NI<3, T, C_IE_L_B_3<T, ImplT>, ImplT>;
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_ALIAS_H_
