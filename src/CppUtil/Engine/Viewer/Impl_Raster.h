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
		class PLDM_Generator;

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

		class AreaLight;

		class Impl_Raster : public Basic::EleVisitor {
			ELEVISITOR_SETUP(Impl_Raster)
			friend class PLDM_Generator;
		public:
			Impl_Raster(Basic::Ptr<Scene> scene);

			void Draw();
			void Init();

		private:
			void Visit(Basic::Ptr<SObj> sobj);

			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);

			void Visit(Basic::Ptr<BSDF_Diffuse> bsdf);
			void Visit(Basic::Ptr<BSDF_Glass> bsdf);
			void Visit(Basic::Ptr<BSDF_Mirror> bsdf);
			void Visit(Basic::Ptr<BSDF_Emission> bsdf);
			void Visit(Basic::Ptr<BSDF_CookTorrance> bsdf);
			void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf);

			void Visit(Basic::Ptr<AreaLight> areaLight);

		private:
			void InitVAOs();
			void InitVAO_Sphere();
			void InitVAO_Plane();

			void InitShaders();
			void InitShaderBasic();
			void InitShaderDiffuse();
			void InitShaderMetalWorkflow();

			void UpdateLights() const;
			void SetPointLightDepthMap(OpenGL::Shader shader, int base);
			OpenGL::Texture GetTex(Basic::CPtr<Basic::Image> img);
			OpenGL::VAO GetMeshVAO(Basic::CPtr<TriMesh> mesh);

		private:
			std::vector<glm::mat4> modelVec;

		private:
			Basic::Ptr<Scene> scene;

			OpenGL::VAO VAO_P3N3T2T3_Sphere;
			OpenGL::VAO VAO_P3N3T2T3_Plane;
			std::map<Basic::WCPtr<TriMesh>, OpenGL::VAO> meshVAOs;

			OpenGL::Shader shader_basic;
			OpenGL::Shader shader_diffuse;
			OpenGL::Shader shader_metalWorkflow;
			std::map<Basic::WCPtr<Basic::Image>, OpenGL::Texture> img2tex;

			unsigned int lightsUBO;

			OpenGL::Shader curShader;

			Basic::Ptr<PLDM_Generator> pldmGenerator;
			static const int maxPointLights;
		};
	}
}

#endif//!_ENGINE_IMPL_RASTER_H_
