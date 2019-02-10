#ifndef _ENGINE_RTX_RAY_TRACER_H_
#define _ENGINE_RTX_RAY_TRACER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {
		class Scene;
		class Ray;

		class RayTracer : public Basic::HeapObj {
			HEAP_OBJ_SETUP(RayTracer)
		public:
			RayTracer(Basic::Ptr<Scene> scene)
				: scene(scene) { }

			// ray 处于世界坐标系
			virtual glm::vec3 Trace(Basic::Ptr<Ray> ray, int depth = 0) = 0;

			Basic::Ptr<Scene> GetScene() const { return scene; }

		protected:
			Basic::Ptr<Scene> scene;
		};
	}
}

#endif//!_ENGINE_RTX_RAY_TRACER_H_
