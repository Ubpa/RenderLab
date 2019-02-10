#ifndef _ENGINE_RTX_PATH_TRACER_H_
#define _ENGINE_RTX_PATH_TRACER_H_

#include <CppUtil/Engine/RayTracer.h>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class PathTracer : public RayTracer {
			HEAP_OBJ_SETUP(PathTracer)
		public:
			PathTracer(Basic::Ptr<Scene> scene);

			// ray 处于世界坐标系
			virtual glm::vec3 Trace(Basic::Ptr<Ray> ray, int depth = 0);
		
		public:
			int sampleNumForAreaLight;
			int maxDepth;
		};
	}
}

#endif//!_ENGINE_PATH_TRACER_H_
