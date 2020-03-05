#pragma once

#include <Engine/ForwardRaster.h>

namespace CppUtil {
	namespace Engine {
		class DirectIllumRaster final : public ForwardRaster {
		public:
			DirectIllumRaster(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera)
				: ForwardRaster(pOGLW, scene, camera) { }

		public:
			static const Basic::Ptr<DirectIllumRaster> New(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera) {
				return Basic::New<DirectIllumRaster>(pOGLW, scene, camera);
			}

		protected:
			virtual ~DirectIllumRaster() = default;

		public:
			virtual void Init() override;

		protected:
			virtual void Visit(Basic::Ptr<BSDF_Diffuse> bsdf) override;
			virtual void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf) override;
			virtual void Visit(Basic::Ptr<BSDF_FrostedGlass> bsdf) override;

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
