#ifndef _RTX_RENDERER_H_
#define _RTX_RENDERER_H_

#include <CppUtil/Basic/Ptr.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}
}

namespace RTX {
	class RayTracer;
	class Scene;

	class RTX_Renderer {
	public:
		RTX_Renderer(CppUtil::Basic::CPtr<Scene> scene, CppUtil::Basic::Ptr<CppUtil::Basic::Image> img);
		void Run();
		void Stop();
		float ProgressRate();
	private:
		CppUtil::Basic::Ptr<CppUtil::Basic::Image> img;
		CppUtil::Basic::ptr<RayTracer> rayTracer;
		CppUtil::Basic::CPtr<Scene> scene;
		volatile bool isStop;
		size_t maxLoop;
		size_t curLoop;
	};
}


#endif // !_RTX_RENDERER_H_
