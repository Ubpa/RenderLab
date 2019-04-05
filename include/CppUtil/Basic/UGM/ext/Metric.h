#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_H_

#include <CppUtil/Basic/UGM/ext/Metric_Base.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <int N, typename T, MetricType MetricT, typename BaseT, typename ImplT>
			class Metric : public Metric_Base<N, T, MetricT, BaseT, ImplT> {
			public:
				using Metric_Base<N, T, MetricT, BaseT, ImplT>::Metric_Base;

			public:
				T Distance2With(const ImplT & v) const {
					return Distance2(*static_cast<const ImplT*>(this), v);
				}

				static T Distance(const ImplT & lhs, const ImplT & rhs) {
					return sqrt(Distance2(lhs, rhs));
				}

				T DistanceWith(const ImplT & v) const {
					return Distance(*static_cast<const ImplT*>(this), v);
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_H_
