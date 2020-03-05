#ifndef _APP_SOBJ_SAMPLER_RTX_SAMPLER_H_
#define _APP_SOBJ_SAMPLER_RTX_SAMPLER_H_

#include <Basic/HeapObj.h>

#include <UGM/point.h>
#include <UGM/val.h>

#include <functional>
#include <vector>

namespace Ubpa {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class Scene;

		class RayTracer;
	}
}

namespace App {
	class RTX_Sampler : public HeapObj {
	public:
		RTX_Sampler(const std::function<Ptr<RayTracer>()> & generator, int maxLoop, int sampleNum);

	public:
		static const Ptr<RTX_Sampler> New(
			const std::function<Ptr<RayTracer>()> & generator,
			int maxLoop, int sampleNum)
		{
			return New<RTX_Sampler>(generator, maxLoop, sampleNum);
		}

	protected:
		virtual ~RTX_Sampler() = default;

	public:
		void Run(Ptr<Scene> scene, Ptr<Image> img);

		const std::vector<std::vector<Ubpa::valu2>> & GetJobs() const { return jobs; }

	private:
		std::function<Ptr<RayTracer>()> generator;
		std::vector<Ptr<RayTracer>> rayTracers;

		std::vector<std::vector<Ubpa::valu2>> jobs;

		int maxLoop;
		int sampleNum;

		const int threadNum;
	};
}

#endif//!_APP_SOBJ_SAMPLER_RTX_SAMPLER_H_
