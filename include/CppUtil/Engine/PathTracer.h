#ifndef _ENGINE_RTX_PATH_TRACER_H_
#define _ENGINE_RTX_PATH_TRACER_H_

#include <CppUtil/Engine/RayTracer.h>

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

#include <vector>
#include <map>

namespace CppUtil {
	namespace Engine {
		class LightBase;
		class BVHAccel;

		class RayIntersector;
		class VisibilityChecker;

		class BSDF;

		// 只能用于单线程
		class PathTracer : public RayTracer {
			HEAP_OBJ_SETUP(PathTracer)
		public:
			PathTracer();

			virtual glm::vec3 Trace(Basic::Ptr<Ray> ray) { return Trace(ray, 0, glm::vec3(1)); }

			virtual void Init(Basic::Ptr<Scene> scene);

		protected:
			// ray 处于世界坐标系
			glm::vec3 Trace(Basic::Ptr<Ray> ray, int depth, glm::vec3 pathThroughput);

		private:
			enum SampleLightMode {
				ALL,
				RandomOne,
			};

			glm::vec3 SampleLight(
				const Basic::Ptr<Ray> ray,
				const glm::vec3 & posInWorldSpace,
				const std::vector<glm::vec3> & posInLightSpaceVec,
				const glm::mat3 & worldToSurface,
				const Basic::Ptr<BSDF> bsdf,
				const glm::vec3 & w_out,
				const glm::vec2 & texcoord,
				const SampleLightMode mode
			) const;

			glm::vec3 SampleLightImpl(
				const Basic::Ptr<Ray> ray,
				const int lightID,
				const glm::vec3 & posInWorldSpace,
				const glm::vec3 & posInLightSpace,
				const glm::mat3 & worldToSurface,
				const Basic::Ptr<BSDF> bsdf,
				const glm::vec3 & w_out,
				const glm::vec2 & texcoord,
				const float factorPD
			) const;

			glm::vec3 SampleBSDF(
				const Basic::Ptr<BSDF> bsdf,
				const SampleLightMode mode,
				const glm::vec3 & w_out,
				const glm::mat3 & surfaceToWorld,
				const glm::vec2 & texcoord,
				const std::vector<glm::vec3> & posInLightSpaceVec,
				const Basic::Ptr<Ray> ray,
				const glm::vec3 & hitPos,
				const int depth,
				glm::vec3 pathThroughput
			);
		
		public:
			int maxDepth;

		private:
			std::vector<Basic::Ptr<LightBase>> lights;
			std::map<Basic::Ptr<LightBase>, int> lightToIdx;
			std::vector<glm::mat4> worldToLightVec;
			std::vector<glm::mat3> dir_worldToLightVec;// 只需要旋转方向，所以使用 mat3
			std::vector<glm::mat3> dir_lightToWorldVec;// 只需要旋转方向，所以使用 mat3

			Basic::Ptr<BVHAccel> bvhAccel;

			Basic::Ptr<RayIntersector> rayIntersector;
			Basic::Ptr<VisibilityChecker> visibilityChecker;
		};
	}
}

#endif//!_ENGINE_PATH_TRACER_H_
