#ifndef _CPPUTIL_ENGINE_VIEWER_PLDM_GENERATOR_H_
#define _CPPUTIL_ENGINE_VIEWER_PLDM_GENERATOR_H_

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

	namespace Engine {
		class Scene;
		class SObj;

		class Sphere;
		class Plane;
		class TriMesh;

		class CmptLight;

		class PointLight;
		class DirectionalLight;
		class SpotLight;

		// Point Light Depth Map Generator
		class PLDM_Generator: public Basic::Visitor {
		public:
			PLDM_Generator(QT::RawAPI_OGLW * pOGLW, float lightNear, float lightFar);

		public:
			static const Basic::Ptr<PLDM_Generator> New(QT::RawAPI_OGLW * pOGLW, float lightNear, float lightFar) {
				return Basic::New<PLDM_Generator>(pOGLW, lightNear, lightFar);
			}

		protected:
			virtual ~PLDM_Generator() = default;
			
		public:
			void Init();
			const OpenGL::Texture GetDepthCubeMap(Basic::PtrC<CmptLight> light) const;

		private:
			void Visit(Basic::Ptr<Scene> scene);
			void Visit(Basic::Ptr<SObj> sobj);

			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			QT::RawAPI_OGLW * pOGLW;

			struct FBO_Tex {
				FBO_Tex(const OpenGL::FBO & fbo = OpenGL::FBO(), const OpenGL::Texture & tex = OpenGL::Texture())
					: fbo(fbo), tex(tex) { }

				OpenGL::FBO fbo;
				OpenGL::Texture tex;
			};
			std::map<Basic::WPtrC<CmptLight>, FBO_Tex> lightMap;
			int depthMapSize;
			float lightNear;
			float lightFar;

			OpenGL::Shader shader_genDepth;
			std::vector<Transform> modelVec;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_VIEWER_PLDM_GENERATOR_H_
