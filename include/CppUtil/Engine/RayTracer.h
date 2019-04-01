#ifndef _ENGINE_RTX_RAY_TRACER_H_
#define _ENGINE_RTX_RAY_TRACER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Basic/RGB.h>

namespace CppUtil {
	namespace Engine {
		class Scene;

		class RayTracer : public Basic::HeapObj {
			HEAP_OBJ_SETUP(RayTracer)
		public:
			// ray 处于世界坐标系
			virtual const RGBf Trace(Ray & ray) = 0;
			virtual void Init(Basic::Ptr<Scene> scene) {}
		};
	}
}

#endif//!_ENGINE_RTX_RAY_TRACER_H_
