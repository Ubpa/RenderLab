#pragma once

#include <Basic/HeapObj.h>

#include <Engine/Ray.h>

#include <UGM/rgb.h>

namespace CppUtil {
	namespace Engine {
		class Scene;
		class BVHAccel;

		class RayTracer : public Basic::HeapObj {
		protected:
			RayTracer() = default;
			virtual ~RayTracer() = default;

		public:
			// ray 处于世界坐标系
			virtual const Ubpa::rgbf Trace(Ray & ray) = 0;
			virtual void Init(Basic::Ptr<Scene> scene, Basic::Ptr<BVHAccel> bvhAccel) {
				this->bvhAccel = bvhAccel;
			}

		protected:
			Basic::Ptr<BVHAccel> bvhAccel;
		};
	}
}
