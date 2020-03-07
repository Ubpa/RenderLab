#pragma once

#include <Basic/HeapObj.h>

#include <Engine/Viewer/Ray.h>

#include <UGM/rgb.h>

namespace Ubpa {
	class Scene;
	class BVHAccel;

	class RayTracer : public HeapObj {
	protected:
		RayTracer() = default;
		virtual ~RayTracer() = default;

	public:
		// ray 处于世界坐标系
		virtual const rgbf Trace(Ray& ray) = 0;
		virtual void Init(Ptr<Scene> scene, Ptr<BVHAccel> bvhAccel) {
			this->bvhAccel = bvhAccel;
		}

	protected:
		Ptr<BVHAccel> bvhAccel;
	};
}
