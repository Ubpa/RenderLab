#ifndef _ENGINE_VIEWER_ROAMER_H_
#define _ENGINE_VIEWER_ROAMER_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Qt {
		class RawAPI_OGLW;
	}

	namespace OpenGL {
		class Camera;
	}

	namespace Engine {
		class Roamer : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Roamer)
		public:
			Roamer(Qt::RawAPI_OGLW * pOGLW);

			void Init();
			void SetWH(int w, int h);

			Qt::RawAPI_OGLW * GetOGLW() { return pOGLW; }
			Basic::Ptr<OpenGL::Camera> GetCamera() { return camera; }
		private:
			void ListenerInit();
			void UpdateCamera();

			Qt::RawAPI_OGLW * pOGLW;
			unsigned int cameraMatrixsUBO;
			Basic::Ptr<OpenGL::Camera> camera;
		};
	}
}

#endif//!_ENGINE_VIEWER_ROAMER_H_
