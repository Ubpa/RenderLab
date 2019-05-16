#ifndef _ENGINE_VIEWER_RASTER_H_
#define _ENGINE_VIEWER_RASTER_H_

#include <CppUtil/Engine/RasterBase.h>

namespace CppUtil {
	namespace Engine {
		class DirectIllumRaster final : public RasterBase {
		public:
			DirectIllumRaster(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene) : RasterBase(pOGLW, scene) { }

		public:
			static const Basic::Ptr<DirectIllumRaster> New(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene) {
				return Basic::New<DirectIllumRaster>(pOGLW, scene);
			}

		protected:
			virtual ~DirectIllumRaster() = default;

		public:
			virtual void OGL_Init() override;

		protected:
			virtual void Visit(Basic::Ptr<BSDF_Diffuse> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_FrostedGlass> bsdf);

		private:
			void InitShaders();
			void InitShaderDiffuse();
			void InitShaderMetalWorkflow();
			void InitShaderFrostedGlass();

		private:
			OpenGL::Shader shader_diffuse;
			OpenGL::Shader shader_metalWorkflow;
			OpenGL::Shader shader_frostedGlass;
		};
	}
}

#endif//!_ENGINE_VIEWER_RASTER_H_
