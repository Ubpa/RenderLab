#ifndef _APP_SOBJ_SAMPLER_RTX_SAMPLER_H_
#define _APP_SOBJ_SAMPLER_RTX_SAMPLER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <CppUtil/Basic/UGM/Point2.h>

#include <functional>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class Scene;

		class RayTracerBase;
	}
}

namespace App {
	class RTX_Sampler : public CppUtil::Basic::HeapObj<RTX_Sampler> {
	public:
		RTX_Sampler(const std::function<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracerBase>()> & generator, int maxLoop, int sampleNum);

	public:
		static const CppUtil::Basic::Ptr<RTX_Sampler> New(
			const std::function<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracerBase>()> & generator,
			int maxLoop, int sampleNum)
		{
			return CppUtil::Basic::New<RTX_Sampler>(generator, maxLoop, sampleNum);
		}

	protected:
		virtual ~RTX_Sampler() = default;

	public:
		void Run(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene, CppUtil::Basic::Ptr<CppUtil::Basic::Image> img);

		const std::vector<std::vector<CppUtil::Point2ui>> & GetJobs() const { return jobs; }

	private:
		std::function<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracerBase>()> generator;
		std::vector<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracerBase>> rayTracers;

		std::vector<std::vector<CppUtil::Point2ui>> jobs;

		int maxLoop;
		int sampleNum;

		const int threadNum;
	};
}

#endif//!_APP_SOBJ_SAMPLER_RTX_SAMPLER_H_
