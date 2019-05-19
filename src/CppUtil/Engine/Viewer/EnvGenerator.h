#ifndef _CPPUTIL_ENGINE_VIEWER_CM_GENERATOR_H_
#define _CPPUTIL_ENGINE_VIEWER_CM_GENERATOR_H_

#include <CppUtil/Basic/Visitor.h>
#include <CppUtil/OpenGL/FBO.h>
#include <CppUtil/OpenGL/Texture.h>
#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/Shader.h>

#include <CppUtil/Basic/UGM/Transform.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace QT {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class Scene;
		class SObj;

		class Sphere;
		class Plane;
		class TriMesh;

		class InfiniteAreaLight;

		// Cubemap Generator, GPU-driven
		class EnvGenerator : public Basic::Visitor {
		public:
			EnvGenerator(QT::RawAPI_OGLW * pOGLW);

		public:
			static const Basic::Ptr<EnvGenerator> New(QT::RawAPI_OGLW * pOGLW) {
				return Basic::New<EnvGenerator>(pOGLW);
			}

		protected:
			virtual ~EnvGenerator() = default;

		public:
			void OGL_Init();
			const OpenGL::Texture GetSkybox(Basic::PtrC<Basic::Image> img) const;
			const OpenGL::Texture GetIrradianceMap(Basic::PtrC<Basic::Image> img) const;

		private:
			void Visit(Basic::Ptr<Scene> scene);

		private:
			void InitShader_genSkybox();
			void InitShader_genIrradiance();

			void Clear();

		private:
			QT::RawAPI_OGLW * pOGLW;

			int skyboxSize;
			int irradianceSize;

			OpenGL::Shader shader_genSkybox;
			OpenGL::Shader shader_genIrradiance;

			OpenGL::FBO genSkyboxFBO;
			OpenGL::FBO genIrradianceFBO;

			Basic::WPtrC<Basic::Image> curImg;
			OpenGL::Texture skybox;
			OpenGL::Texture irradianceMap;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_VIEWER_CM_GENERATOR_H_
