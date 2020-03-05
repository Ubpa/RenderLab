#ifndef _APP_SOBJ_SAMPLER_RTX_SAMPLER_H_
#define _APP_SOBJ_SAMPLER_RTX_SAMPLER_H_

#include <Basic/HeapObj.h>

#include <UGM/point.h>
#include <UGM/val.h>

#include <functional>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class Scene;

		class RayTracer;
	}
}

namespace App {
	class RTX_Sampler : public CppUtil::Basic::HeapObj {
	public:
		RTX_Sampler(const std::function<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracer>()> & generator, int maxLoop, int sampleNum);

	public:
		static const CppUtil::Basic::Ptr<RTX_Sampler> New(
			const std::function<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracer>()> & generator,
			int maxLoop, int sampleNum)
		{
			return CppUtil::Basic::New<RTX_Sampler>(generator, maxLoop, sampleNum);
		}

	protected:
		virtual ~RTX_Sampler() = default;

	public:
		void Run(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene, CppUtil::Basic::Ptr<CppUtil::Basic::Image> img);

		const std::vector<std::vector<Ubpa::valu2>> & GetJobs() const { return jobs; }

	private:
		std::function<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracer>()> generator;
		std::vector<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracer>> rayTracers;

		std::vector<std::vector<Ubpa::valu2>> jobs;

		int maxLoop;
		int sampleNum;

		const int threadNum;
	};
}

#endif//!_APP_SOBJ_SAMPLER_RTX_SAMPLER_H_
