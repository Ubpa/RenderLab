#ifndef _ENGINE_RAY_TRACER_H_
#define _ENGINE_RAY_TRACER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {
		class Ray;
		class RayTracer : public Basic::HeapObj {
			HEAP_OBJ_SETUP(RayTracer)
		public:
			virtual glm::vec3 Trace(Basic::Ptr<Ray> ray) = 0;
		};
	}
}

#endif//!_ENGINE_RAY_TRACER_H_
