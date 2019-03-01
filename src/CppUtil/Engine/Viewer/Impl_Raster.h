#ifndef _ENGINE_IMPL_RASTER_H_
#define _ENGINE_IMPL_RASTER_H_

#include <CppUtil/Basic/EleVisitor.h>

#include <glm/mat4x4.hpp>

#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/OpenGL/Texture.h>

#include <map>

namespace CppUtil {
	namespace Basic {
		class Operation;
		class Image;
	}

	namespace Qt {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class Scene;

		class SObj;

		class Sphere;
		class Plane;
		class TriMesh;

		class BSDF_Diffuse;
		class BSDF_Mirror;
		class BSDF_Glass;
		class BSDF_Emission;
		class BSDF_CookTorrance;
		class BSDF_MetalWorkflow;

		class Impl_Raster : public Basic::EleVisitor {
			ELEVISITOR_SETUP_FUNCNAME(Impl_Raster, Draw)
		public:
			Impl_Raster(Basic::Ptr<Scene> scene);

			void Draw();
			void Init();

		private:
			void Draw(Basic::Ptr<SObj> sobj);

			void Draw(Basic::Ptr<Sphere> sphere);
			void Draw(Basic::Ptr<Plane> plane);
			void Draw(Basic::Ptr<TriMesh> mesh);

			void Draw(Basic::Ptr<BSDF_Diffuse> bsdf);
			void Draw(Basic::Ptr<BSDF_Glass> bsdf);
			void Draw(Basic::Ptr<BSDF_Mirror> bsdf);
			void Draw(Basic::Ptr<BSDF_Emission> bsdf);
			void Draw(Basic::Ptr<BSDF_CookTorrance> bsdf);
			void Draw(Basic::Ptr<BSDF_MetalWorkflow> bsdf);

		private:
			void UpdateLights() const;
			OpenGL::Texture GetTex(Basic::CPtr<Basic::Image> img);

		private:
			std::vector<glm::mat4> modelVec;

		private:
			Basic::Ptr<Scene> scene;
			OpenGL::VAO VAO_P3_Sphere;
			OpenGL::VAO VAO_P3_Plane;
			OpenGL::Shader shader_basic;
			OpenGL::Shader shader_diffuse;
			std::map<Basic::Ptr<TriMesh>, OpenGL::VAO> meshVAOs;
			std::map<Basic::WCPtr<Basic::Image>, OpenGL::Texture> img2tex;

			unsigned int lightsUBO;

			OpenGL::Shader curShader;
		};
	}
}

#endif//!_ENGINE_IMPL_RASTER_H_
