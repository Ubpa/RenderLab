#pragma once

#include <Basic/Element.h>
#include <UGM/bbox.h>

namespace Ubpa {
	class Primitive;

	class Shape : public Element {
	protected:
		Shape() = default;
		virtual ~Shape() = default;

	public:
		virtual const bboxf3 GetBBox() const = 0;
		virtual const Ptr<Primitive> GetPrimitive() = 0;
	};
}
