#pragma once

#include <Engine/ForwardRaster.h>

namespace CppUtil {
	namespace Engine {
		class WireframeRaster : public ForwardRaster {
		public:
			WireframeRaster(QT::RawAPI_OGLW* pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera)
				: ForwardRaster(pOGLW, scene, camera) { }

		public:
			static Basic::Ptr<WireframeRaster> New(QT::RawAPI_OGLW* pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera) {
				return Basic::New<WireframeRaster>(pOGLW, scene, camera);
			}

		protected:
			virtual ~WireframeRaster() = default;

		public:
			virtual void Init() override;
		};
	}
}
