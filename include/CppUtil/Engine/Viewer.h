#ifndef _ENGINE_VIEWER_VIEWER_H_
#define _ENGINE_VIEWER_VIEWER_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Qt {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class Scene;
		class Raster;
		class Roamer;

		class Viewer : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Viewer)
		public:
			Viewer(Qt::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene);

		public:
			Qt::RawAPI_OGLW * GetOGLW() const { return pOGLW; }
			Basic::Ptr<Scene> GetScene() const { return scene; }
			Basic::Ptr<Raster> GetRaster() const { return raster; }
			Basic::Ptr<Roamer> GetRoamer() const { return roamer; }

		private:
			Qt::RawAPI_OGLW * pOGLW;
			Basic::Ptr<Scene> scene;
			Basic::Ptr<Raster> raster;
			Basic::Ptr<Roamer> roamer;
		};
	}
}

#endif//!_ENGINE_VIEWER_VIEWER_H_
