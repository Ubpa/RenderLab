#ifndef _ENGINE_RTX_PATH_TRACER_H_
#define _ENGINE_RTX_PATH_TRACER_H_

#include <CppUtil/Engine/RayTracer.h>

#include <CppUtil/Basic/UGM/Transform.h>
#include <CppUtil/Basic/UGM/Mat3x3.h>

#include <vector>
#include <map>

namespace CppUtil {
	namespace Engine {
		class LightBase;
		class BVHAccel;

		class RayIntersector;
		class VisibilityChecker;

		class BSDFBase;

		// 只能用于单线程
		class PathTracer : public RayTracer<PathTracer> {
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
				const Point3 & posInWorldSpace,
				const std::vector<Point3> & posInLightSpaceVec,
				const Mat3f & worldToSurface,
				Basic::Ptr<BSDFBase> bsdf,
				const Normalf & w_out,
				const Point2 & texcoord,
				SampleLightMode mode
			) const;

			const RGBf SampleLightImpl(
				Ray & ray,
				int lightID,
				const Point3 & posInWorldSpace,
				const Point3 & posInLightSpace,
				const Mat3f & worldToSurface,
				Basic::Ptr<BSDFBase> bsdf,
				const Normalf & w_out,
				const Point2 & texcoord,
				float factorPD
			) const;

			const RGBf SampleBSDF(
				Basic::Ptr<BSDFBase> bsdf,
				SampleLightMode mode,
				const Normalf & w_out,
				const Mat3f & surfaceToWorld,
				const Point2 & texcoord,
				const std::vector<Point3> & posInLightSpaceVec,
				Ray & ray,
				const Point3 & hitPos,
				int depth,
				RGBf pathThroughput
			);
		
		public:
			int maxDepth;

		private:
			std::vector<Basic::Ptr<LightBase>> lights;
			std::map<Basic::Ptr<LightBase>, int> lightToIdx;
			std::vector<Transform> worldToLightVec;
			std::vector<Transform> lightToWorldVec;

			Basic::Ptr<BVHAccel> bvhAccel;

			Basic::Ptr<RayIntersector> rayIntersector;
			Basic::Ptr<VisibilityChecker> visibilityChecker;
		};
	}
}

#endif//!_ENGINE_PATH_TRACER_H_
