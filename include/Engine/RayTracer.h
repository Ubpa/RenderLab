#ifndef _ENGINE_RTX_RAY_TRACER_H_
#define _ENGINE_RTX_RAY_TRACER_H_

#include <Basic/HeapObj.h>

#include <Engine/Ray.h>

#include <Basic/UGM/RGB.h>

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
			virtual const RGBf Trace(Ray & ray) = 0;
			virtual void Init(Basic::Ptr<Scene> scene, Basic::Ptr<BVHAccel> bvhAccel) {
				this->bvhAccel = bvhAccel;
			}

		protected:
			Basic::Ptr<BVHAccel> bvhAccel;
		};
	}
}

#endif//!_ENGINE_RTX_RAY_TRACER_H_
