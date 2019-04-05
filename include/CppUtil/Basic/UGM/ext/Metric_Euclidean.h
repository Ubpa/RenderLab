#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_VAL_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_VAL_H_

#include <CppUtil/Basic/UGM/ext/Metric_EuclideanBase.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <int N, typename T, typename MetricT, typename BaseT, typename ImplT>
			class EulideanMetric : public EulideanMetric_Base<N, T, BaseT, ImplT> {
			public:
				using EulideanMetric_Base<N, T, BaseT, ImplT>::EulideanMetric_Base;

			public:
				T Distance2With(const ImplT & v) const {
					return Distance2(*static_cast<ImplT*>(this), v);
				}

				static T Distance(const ImplT & lhs, const ImplT & rhs) {
					return sqrt(Distance2(lhs, rhs));
				}

				T DistanceWith(const ImplT & v) const {
					return Distance(*static_cast<ImplT*>(this), v);
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_LINEARITY_VAL_H_
