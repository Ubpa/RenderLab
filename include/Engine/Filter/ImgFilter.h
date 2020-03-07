#pragma once

#include <Basic/HeapObj.h>

#include <UGM/point.h>
#include <UGM/vec.h>

namespace Ubpa {
	class ImgFilter : public HeapObj {
	protected:
		ImgFilter(const vecf2& radius) :
			radius(radius), invRadius(1.0f / radius[0], 1.0f / radius[1]) { }
		virtual ~ImgFilter() = default;

	public:
		virtual float Evaluate(const pointf2& p) const = 0;

	public:
		const vecf2 radius;
		const vecf2 invRadius;
	};
}
