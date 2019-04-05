#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_BASE_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_BASE_H_

#include <CppUtil/Basic/UGM/ext/Metric_Base.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			enum MetricType {
				Euclidean,
			};

			template <int N, typename T, MetricType metricT, typename BaseT, typename ImplT>
			class Metric_Base;
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_BASE_H_
