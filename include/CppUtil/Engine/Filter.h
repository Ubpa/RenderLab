#ifndef _CPPUTIL_ENGINE_FILTER_FILTER_H_
#define _CPPUTIL_ENGINE_FILTER_FILTER_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/UGM/Point2.h>

namespace CppUtil {
	namespace Engine {
		class Filter : public Basic::HeapObj {
		protected:
			Filter(const Vec2 & radius) :
				radius(radius), invRadius(1.0f / radius.x, 1.0f / radius.y) { }
			virtual ~Filter() = default;

		public:
			virtual float Evaluate(const Point2f & p) const = 0;

		public:
			const Vec2 radius;
			const Vec2 invRadius;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_FILTER_FILTER_H_
