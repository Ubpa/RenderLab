#ifndef _ENGINE_RTX_RTX_RENDERER_H_
#define _ENGINE_RTX_RTX_RENDERER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <3rdParty/enum.h>

#include <functional>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class Scene;

		class RayTracer;

		BETTER_ENUM(RendererState, int, Running, Stop)

		class RTX_Renderer : public Basic::HeapObj {
		public:
			RTX_Renderer(const std::function<Basic::Ptr<RayTracer>()> & generator);
			
		public:
			static const Basic::Ptr<RTX_Renderer> New(const std::function<Basic::Ptr<RayTracer>()> & generator) {
				return Basic::New<RTX_Renderer>(generator);
			}

		public:
			void Run(Basic::Ptr<Scene> scene, Basic::Ptr<Basic::Image> img);
			void Stop();
			RendererState GetState() const { return state; }
			float ProgressRate();

		public:
			volatile int maxLoop;

		private:
			std::function<Basic::Ptr<RayTracer>()> generator;

			RendererState state;
			int curLoop;

			const int threadNum;
		};
	}
}

#endif//!_ENGINE_RTX_RTX_RENDERER_H_
