#pragma once

#include <Basic/HeapObj.h>

#include <UGM/bbox.h>

namespace Ubpa {
	class Primitive;

	class Shape : public HeapObj {
	protected:
		Shape() = default;
		virtual ~Shape() = default;

	public:
		virtual const bboxf3 GetBBox() const = 0;
		virtual const Ptr<Primitive> GetPrimitive() = 0;
	};
}
