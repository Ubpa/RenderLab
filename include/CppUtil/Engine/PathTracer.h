#ifndef _ENGINE_PATH_TRACER_H_
#define _ENGINE_PATH_TRACER_H_

#include <CppUtil/Engine/RayTracer.h>

namespace CppUtil {
	namespace Engine {
		class PathTracer : public RayTracer {
			HEAP_OBJ_SETUP(PathTracer)
		public:
			virtual glm::vec3 Trace(Basic::Ptr<Ray> ray);

		private:

		};
	}
}

#endif//!_ENGINE_PATH_TRACER_H_
