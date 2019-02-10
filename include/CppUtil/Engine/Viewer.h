#ifndef _ENGINE_VIEWER_VIEWER_H_
#define _ENGINE_VIEWER_VIEWER_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/Operation.h>

namespace CppUtil {
	namespace OpenGL {
		class Camera;
	}

	namespace Qt {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class Scene;
		class Raster;

		class Viewer : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Viewer)
		public:
			Viewer(Qt::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene);

		public:
			Qt::RawAPI_OGLW * GetOGLW() const { return pOGLW; }
			Basic::Ptr<Scene> GetScene() const { return scene; }

		public:
			void ListenerInit();
			Basic::Ptr<OpenGL::Camera> GetCamera() const { return camera; }
			Basic::Ptr<Raster> GetRaster() const { return raster; }
			size_t GetCamUBO() const { return cameraMatrixsUBO; }

		private:
			Qt::RawAPI_OGLW * pOGLW;
			Basic::Ptr<Scene> scene;
			Basic::Ptr<Raster> raster;
			Basic::Ptr<OpenGL::Camera> camera;
			size_t cameraMatrixsUBO;
		};
	}
}

#endif//!_ENGINE_VIEWER_VIEWER_H_
