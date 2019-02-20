#ifndef _ENGINE_RTX_RTX_RENDERER_H_
#define _ENGINE_RTX_RTX_RENDERER_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class RayTracer;
		class Scene;

		class RTX_Renderer : public Basic::HeapObj {
			HEAP_OBJ_SETUP(RTX_Renderer)
		public:
			RTX_Renderer(Basic::Ptr<RayTracer> rayTracer);

		public:
			void Run(Basic::Ptr<Basic::Image> img);
			void Stop();
			float ProgressRate();

		public:
			volatile int maxLoop;

		private:
			Basic::Ptr<RayTracer> rayTracer;

			volatile bool isStop;
			int curLoop;
		};
	}
}

#endif//!_ENGINE_RTX_RTX_RENDERER_H_
