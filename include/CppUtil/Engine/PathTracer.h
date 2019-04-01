#ifndef _ENGINE_RTX_PATH_TRACER_H_
#define _ENGINE_RTX_PATH_TRACER_H_

#include <CppUtil/Engine/RayTracer.h>

#include <CppUtil/Basic/Transform.h>
#include <CppUtil/Basic/Mat3x3.h>

#include <vector>
#include <map>

namespace CppUtil {
	namespace Engine {
		class Light;
		class BVHAccel;

		class RayIntersector;
		class VisibilityChecker;

		class BSDF;

		// 只能用于单线程
		class PathTracer : public RayTracer {
			HEAP_OBJ_SETUP(PathTracer)
		public:
			PathTracer();

			virtual const RGBf Trace(Ray & ray) { return Trace(ray, 0, RGBf(1.f)); }

			virtual void Init(Basic::Ptr<Scene> scene);

		protected:
			// ray 处于世界坐标系
			const RGBf Trace(Ray & ray, int depth, RGBf pathThroughput);

		private:
			enum SampleLightMode {
				ALL,
				RandomOne,
			};

			const RGBf SampleLight(
				Ray & ray,
				const Pointf & posInWorldSpace,
				const std::vector<Pointf> & posInLightSpaceVec,
				const Mat3f & worldToSurface,
				Basic::Ptr<BSDF> bsdf,
				const Normalf & w_out,
				const Point2f & texcoord,
				SampleLightMode mode
			) const;

			const RGBf SampleLightImpl(
				Ray & ray,
				int lightID,
				const Pointf & posInWorldSpace,
				const Pointf & posInLightSpace,
				const Mat3f & worldToSurface,
				Basic::Ptr<BSDF> bsdf,
				const Normalf & w_out,
				const Point2f & texcoord,
				float factorPD
			) const;

			const RGBf SampleBSDF(
				Basic::Ptr<BSDF> bsdf,
				SampleLightMode mode,
				const Normalf & w_out,
				const Mat3f & surfaceToWorld,
				const Point2f & texcoord,
				const std::vector<Pointf> & posInLightSpaceVec,
				Ray & ray,
				const Pointf & hitPos,
				int depth,
				RGBf pathThroughput
			);
		
		public:
			int maxDepth;

		private:
			std::vector<Basic::Ptr<Light>> lights;
			std::map<Basic::Ptr<Light>, int> lightToIdx;
			std::vector<Transform> worldToLightVec;
			std::vector<Transform> lightToWorldVec;

			Basic::Ptr<BVHAccel> bvhAccel;

			Basic::Ptr<RayIntersector> rayIntersector;
			Basic::Ptr<VisibilityChecker> visibilityChecker;
		};
	}
}

#endif//!_ENGINE_PATH_TRACER_H_
