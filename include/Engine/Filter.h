#pragma once

#include <Basic/HeapObj.h>
#include <UGM/point.h>

namespace CppUtil {
	namespace Engine {
		class Filter : public Basic::HeapObj {
		protected:
			Filter(const Ubpa::vecf2 & radius) :
				radius(radius), invRadius(1.0f / radius[0], 1.0f / radius[1]) { }
			virtual ~Filter() = default;

		public:
			virtual float Evaluate(const Ubpa::pointf2 & p) const = 0;

		public:
			const Ubpa::vecf2 radius;
			const Ubpa::vecf2 invRadius;
		};
	}
}
