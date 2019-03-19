#ifndef _APP_SOBJ_SAMPLER_RTX_SAMPLER_H_
#define _APP_SOBJ_SAMPLER_RTX_SAMPLER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <glm/vec2.hpp>

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
		HEAP_OBJ_SETUP(RTX_Sampler)
	public:
		RTX_Sampler(const std::function<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracer>()> & generator, int maxLoop, int sampleNum);

	public:
		void Run(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene, CppUtil::Basic::Ptr<CppUtil::Basic::Image> img);
		float RTX_Sampler::ProgressRate() const;

		const std::vector<std::vector<glm::uvec2>> & GetJobs() const { return jobs; }

	private:
		std::function<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracer>()> generator;
		std::vector<CppUtil::Basic::Ptr<CppUtil::Engine::RayTracer>> rayTracers;

		std::vector<std::vector<glm::uvec2>> jobs;

		int curLoop;
		int maxLoop;
		int sampleNum;
		const int threadNum;
	};
}

#endif//!_APP_SOBJ_SAMPLER_RTX_SAMPLER_H_
