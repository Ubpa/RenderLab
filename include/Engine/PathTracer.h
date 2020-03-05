#ifndef _ENGINE_RTX_PATH_TRACER_H_
#define _ENGINE_RTX_PATH_TRACER_H_

#include <Engine/RayTracer.h>

#include <UGM/transform.h>
#include <UGM/mat.h>

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
		public:
			PathTracer();

		public:
			static const Basic::Ptr<PathTracer> New() { return Basic::New<PathTracer>(); }

		protected:
			virtual ~PathTracer() = default;

		public:
			virtual const Ubpa::rgbf Trace(Ray & ray) { return Trace(ray, 0, Ubpa::rgbf(1.f)); }

			virtual void Init(Basic::Ptr<Scene> scene, Basic::Ptr<BVHAccel> bvhAccel) override;

		protected:
			// ray 处于世界坐标系
			const Ubpa::rgbf Trace(Ray & ray, int depth, Ubpa::rgbf pathThroughput);

		private:
			enum SampleLightMode {
				ALL,
				RandomOne,
			};

			const Ubpa::rgbf SampleLight(
				const Ubpa::pointf3 & posInWorldSpace,
				const Ubpa::matf3 & worldToSurface,
				Basic::Ptr<BSDF> bsdf,
				const Ubpa::normalf & w_out,
				const Ubpa::pointf2 & texcoord,
				SampleLightMode mode
			) const;

			const Ubpa::rgbf SampleLightImpl(
				int lightID,
				const Ubpa::pointf3 & posInWorldSpace,
				const Ubpa::pointf3 & posInLightSpace,
				const Ubpa::matf3 & worldToSurface,
				Basic::Ptr<BSDF> bsdf,
				const Ubpa::normalf & w_out,
				const Ubpa::pointf2 & texcoord,
				float factorPD
			) const;

			const Ubpa::rgbf SampleBSDF(
				Basic::Ptr<BSDF> bsdf,
				SampleLightMode mode,
				const Ubpa::normalf & w_out,
				const Ubpa::matf3 & surfaceToWorld,
				const Ubpa::pointf2 & texcoord,
				const Ubpa::pointf3 & hitPos,
				int depth,
				Ubpa::rgbf pathThroughput
			);
		
		public:
			int maxDepth;

		private:
			std::vector<Basic::Ptr<Light>> lights;
			std::map<Basic::Ptr<Light>, int> lightToIdx;
			std::vector<Ubpa::transformf> worldToLightVec;
			std::vector<Ubpa::transformf> lightToWorldVec;

			Basic::Ptr<RayIntersector> rayIntersector;
			Basic::Ptr<VisibilityChecker> visibilityChecker;
		};
	}
}

#endif//!_ENGINE_PATH_TRACER_H_
