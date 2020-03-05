#pragma once

#include <UGM/ray.h>

namespace Ubpa {
	class Ray : public rayf3 {
	public:
		Ray(const pointf3& origin = pointf3(0.f, 0.f, 0.f), const vecf3& dir = vecf3(1.f, 1.f, 1.f), float tMin = 0.001f, float tMax = FLT_MAX)
			: rayf3(origin, dir), tMin(tMin), tMax(tMax) { }

	public:
		const pointf3 StartPos() const { return (*this)(tMin); }
		const pointf3 EndPos() const { return (*this)(tMax); }

		const valf3 InvDir() const { return{ 1.f / d[0], 1.f / d[1], 1.f / d[2] }; }

	public:
		float tMin;
		float tMax;
	};
}
