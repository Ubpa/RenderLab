#ifndef _CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_EUCLIDEAN_2_H_
#define _CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_EUCLIDEAN_2_H_

#include <CppUtil/Basic/UGM/ext/Metric.h>

namespace CppUtil {
	namespace Basic {
		namespace EXT {
			template <typename T, typename BaseT, typename ImplT>
			class Metric_Base<2, T, MetricType::Euclidean, BaseT, ImplT> : public BaseT {
			public:
				using BaseT::BaseT;

			public:
				static T Distance2(const ImplT & lhs, const ImplT & rhs) {
					const T dx = lhs.x - rhs.x;
					const T dy = lhs.y - rhs.y;

					return
						dx * dx +
						dy * dy;
				}
			};
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_EXT_METRIC_EUCLIDEAN_2_H_
