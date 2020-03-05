#pragma once

#include <Basic/Element.h>
#include <UGM/bbox.h>

namespace CppUtil {
	namespace Engine {
		class Primitive;

		class Shape : public Basic::Element {
		protected:
			Shape() = default;
			virtual ~Shape() = default;

		public:
			virtual const Ubpa::bboxf3 GetBBox() const = 0;
			virtual const Basic::Ptr<Primitive> GetPrimitive() = 0;
		};
	}

	using EShape = Engine::Shape;
}
