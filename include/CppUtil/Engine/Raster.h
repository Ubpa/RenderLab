#ifndef _ENGINE_VIEWER_RASTER_H_
#define _ENGINE_VIEWER_RASTER_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace OpenGL {
		class Shader;
	}
	namespace Qt {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class Scene;
		class Impl_Raster;

		// 通过 Impl 隐藏私有成员
		class Raster : public Basic::HeapObj {
		public:
			Raster(Qt::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene);
			OpenGL::Shader & GetShader() const;

			void Draw();
			void Init();
			
		private:
			Basic::Ptr<Impl_Raster> pImpl;
		};
	}
}

#endif//!_ENGINE_VIEWER_RASTER_H_
