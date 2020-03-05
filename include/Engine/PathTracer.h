#pragma once

#include <Engine/RayTracer.h>

#include <UGM/transform.h>
#include <UGM/mat.h>

#include <vector>
#include <map>

namespace Ubpa {
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
		static const Ptr<PathTracer> New() { return Ubpa::New<PathTracer>(); }

	protected:
		virtual ~PathTracer() = default;

	public:
		virtual const rgbf Trace(Ray& ray) { return Trace(ray, 0, rgbf(1.f)); }

		virtual void Init(Ptr<Scene> scene, Ptr<BVHAccel> bvhAccel) override;

	protected:
		// ray 处于世界坐标系
		const rgbf Trace(Ray& ray, int depth, rgbf pathThroughput);

	private:
		enum SampleLightMode {
			ALL,
			RandomOne,
		};

		const rgbf SampleLight(
			const pointf3& posInWorldSpace,
			const matf3& worldToSurface,
			Ptr<BSDF> bsdf,
			const normalf& w_out,
			const pointf2& texcoord,
			SampleLightMode mode
		) const;

		const rgbf SampleLightImpl(
			int lightID,
			const pointf3& posInWorldSpace,
			const pointf3& posInLightSpace,
			const matf3& worldToSurface,
			Ptr<BSDF> bsdf,
			const normalf& w_out,
			const pointf2& texcoord,
			float factorPD
		) const;

		const rgbf SampleBSDF(
			Ptr<BSDF> bsdf,
			SampleLightMode mode,
			const normalf& w_out,
			const matf3& surfaceToWorld,
			const pointf2& texcoord,
			const pointf3& hitPos,
			int depth,
			rgbf pathThroughput
		);

	public:
		int maxDepth;

	private:
		std::vector<Ptr<Light>> lights;
		std::map<Ptr<Light>, int> lightToIdx;
		std::vector<transformf> worldToLightVec;
		std::vector<transformf> lightToWorldVec;

		Ptr<RayIntersector> rayIntersector;
		Ptr<VisibilityChecker> visibilityChecker;
	};
}
