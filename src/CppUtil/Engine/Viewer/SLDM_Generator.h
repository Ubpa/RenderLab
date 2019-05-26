#ifndef _CPPUTIL_ENGINE_VIEWER_SLDM_GENERATOR_H_
#define _CPPUTIL_ENGINE_VIEWER_SLDM_GENERATOR_H_

#include <CppUtil/Basic/Visitor.h>
#include <CppUtil/OpenGL/FBO.h>
#include <CppUtil/OpenGL/Texture.h>
#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/Shader.h>

#include <CppUtil/Basic/UGM/Transform.h>

namespace CppUtil {
	namespace QT {
		class RawAPI_OGLW;
	}

	namespace OpenGL {
		class Camera;
	}

	namespace Engine {
		class Scene;
		class SObj;

		class Sphere;
		class Plane;
		class TriMesh;

		class CmptLight;

		// Spot Light Depth Map Generator
		class SLDM_Generator : public Basic::Visitor {
		public:
			SLDM_Generator(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<OpenGL::Camera> camera, float lightNear, float lightFar);

		public:
			static const Basic::Ptr<SLDM_Generator> New(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<OpenGL::Camera> camera, float lightNear, float lightFar) {
				return Basic::New<SLDM_Generator>(pOGLW, camera, lightNear, lightFar);
			}

		protected:
			virtual ~SLDM_Generator() = default;

		public:
			void Init();
			const OpenGL::Texture GetDepthMap(Basic::PtrC<CmptLight> light) const;
			const Transform GetProjView(Basic::PtrC<CmptLight> light) const;

		private:
			void Visit(Basic::Ptr<Scene> scene);
			void Visit(Basic::Ptr<SObj> sobj);

			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			QT::RawAPI_OGLW * pOGLW;
			Basic::Ptr<OpenGL::Camera> camera;

			struct FBO_Tex {
				FBO_Tex(const OpenGL::FBO & fbo = OpenGL::FBO(), const OpenGL::Texture & tex = OpenGL::Texture())
					: fbo(fbo), tex(tex) { }

				OpenGL::FBO fbo;
				OpenGL::Texture tex;
			};
			std::map<Basic::WPtrC<CmptLight>, FBO_Tex> lightMap;
			std::map<Basic::WPtrC<CmptLight>, Transform> light2pv;
			int depthMapSize;
			float lightNear;
			float lightFar;

			OpenGL::Shader shader_genDepth;
			std::vector<Transform> modelVec;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_VIEWER_SLDM_GENERATOR_H_
