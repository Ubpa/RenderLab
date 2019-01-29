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
	private:
		CppUtil::Basic::Ptr<CppUtil::Basic::Image> img;
		CppUtil::Basic::Ptr<RayTracer> rayTracer;
		CppUtil::Basic::CPtr<Scene> scene;
	};
}


#endif // !_RTX_RENDERER_H_
