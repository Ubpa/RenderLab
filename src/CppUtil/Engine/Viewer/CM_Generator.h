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
		class CM_Generator : public Basic::Visitor {
		public:
			CM_Generator(QT::RawAPI_OGLW * pOGLW);

		public:
			static const Basic::Ptr<CM_Generator> New(QT::RawAPI_OGLW * pOGLW) {
				return Basic::New<CM_Generator>(pOGLW);
			}

		protected:
			virtual ~CM_Generator() = default;

		public:
			void OGL_Init();
			const OpenGL::Texture GetCubeMap(Basic::PtrC<Basic::Image> img) const;

		private:
			void Visit(Basic::Ptr<Scene> scene);

		private:
			QT::RawAPI_OGLW * pOGLW;

			std::map<Basic::WPtrC<Basic::Image>, OpenGL::Texture> img2tex;
			int mapSize;

			OpenGL::Shader shader_genCubemap;

			OpenGL::FBO captureFBO;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_VIEWER_CM_GENERATOR_H_
