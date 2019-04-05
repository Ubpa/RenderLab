#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_ALIAS_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_ALIAS_H_

#include <CppUtil/Basic/UGM/ext/Basic_Val.h>
#include <CppUtil/Basic/UGM/ext/Metric.h>
#include <CppUtil/Basic/UGM/ext/Linearity.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template<int N, typename T, MetricType metricT, typename ImplT>
			using M_B = Metric<N, T, metricT, Basic_Val<N, T, ImplT>, ImplT>;

			template<int N, typename T, typename ImplT>
			using ME_B = M_B<N, T, MetricType::Euclidean, ImplT>;

			template<int N, typename T, typename ImplT>
			using L_B = Linearity<N, T, Basic_Val<N, T, ImplT>, ImplT>;

			template<int N, typename T, MetricType metricT, typename ImplT>
			using L_M_B = Linearity<N, T, M_B<N, T, metricT, ImplT>, ImplT>;

			template<int N, typename T, typename ImplT>
			using L_ME_B = L_M_B<N, T, MetricType::Euclidean, ImplT>;
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_ALIAS_H_
