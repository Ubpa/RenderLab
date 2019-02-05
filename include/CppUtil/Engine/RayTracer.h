#ifndef _ENGINE_RAY_TRACER_H_
#define _ENGINE_RAY_TRACER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {
		class Scene;
		class SObj;
		class Ray;
		class RayIntersector;

		class RayTracer : public Basic::HeapObj {
			HEAP_OBJ_SETUP(RayTracer)
		public:
			RayTracer(Basic::Ptr<Scene> scene);

			// ray 处于世界坐标系
			virtual glm::vec3 Trace(Basic::Ptr<Ray> ray, int depth = 0) = 0;

			Basic::Ptr<Scene> GetScene() const { return scene; }

		protected:
			struct Rst {
				Rst() : closestSObj(nullptr) { }
				Basic::Ptr<SObj> closestSObj;
				glm::vec3 n;
			};

			// ray 所处的坐标系应该是 sobj 所处的坐标系
			void FindClosetSObj(Basic::Ptr<SObj> sobj, Basic::Ptr<Ray> ray, Basic::Ptr<RayIntersector> rayIntersector, Rst & closestRst);

			Basic::Ptr<Scene> scene;
		};
	}
}

#endif//!_ENGINE_RAY_TRACER_H_
