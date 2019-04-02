#ifndef _CPPUTIL_ENGINE_VIEWER_RASTER_BASE_H_
#define _CPPUTIL_ENGINE_VIEWER_RASTER_BASE_H_

#include <CppUtil/Basic/EleVisitor.h>

#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/OpenGL/Texture.h>

#include <CppUtil/Basic/Transform.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
		class Image;
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
		class BSDF_FrostedGlass;

		class RasterBase : public Basic::EleVisitor {
			ELEVISITOR_SETUP(RasterBase)
		public:
			RasterBase(Basic::Ptr<Scene> scene);

		public:// Get Resources
			OpenGL::VAO GetSphereVAO() const { return VAO_P3N3T2T3_Sphere; }
			OpenGL::VAO GetPlaneVAO() const { return VAO_P3N3T2T3_Plane; }
			// get or reg
			OpenGL::Texture GetTex(Basic::CPtr<Basic::Image> img);
			// get or reg
			OpenGL::VAO GetMeshVAO(Basic::CPtr<TriMesh> mesh);

		protected:// Use for SubClass
			void Draw();

			void Init();

			void SetPointLightDepthMap(const OpenGL::Shader & shader, int base);
			void SetCurShader(const OpenGL::Shader & shader) { curShader = shader; }

			// uniform samplerCube pointLightDepthMap0 -- pointLightDepthMap7
			// uniform float lightFar
			void SetShaderForShadow(const OpenGL::Shader & shader, int base) const;

			// Camera 0
			// PointLights 1
			void BindBlock(const OpenGL::Shader & shader) const;

		protected:
			virtual void Visit(Basic::Ptr<SObj> sobj);

			// SetCurShader for Primitive
			virtual void Visit(Basic::Ptr<BSDF_Diffuse> bsdf) {};
			virtual void Visit(Basic::Ptr<BSDF_Glass> bsdf) {};
			virtual void Visit(Basic::Ptr<BSDF_Mirror> bsdf) {};
			virtual void Visit(Basic::Ptr<BSDF_Emission> bsdf) {};
			virtual void Visit(Basic::Ptr<BSDF_CookTorrance> bsdf) {};
			virtual void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf) {};
			virtual void Visit(Basic::Ptr<BSDF_FrostedGlass> bsdf) {};

		private: // no need to change
			/*virtual */void Visit(Basic::Ptr<Sphere> sphere);
			/*virtual */void Visit(Basic::Ptr<Plane> plane);
			/*virtual */void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			void InitVAOs();
			void InitVAO_Sphere();
			void InitVAO_Plane();

			void UpdateLights() const;

		protected:
			Basic::Ptr<Scene> scene;

		private:
			OpenGL::VAO VAO_P3N3T2T3_Sphere;
			OpenGL::VAO VAO_P3N3T2T3_Plane;

			OpenGL::Shader curShader;

			std::vector<Transform> modelVec;

			std::map<Basic::WCPtr<TriMesh>, OpenGL::VAO> meshVAOs;

			std::map<Basic::WCPtr<Basic::Image>, OpenGL::Texture> img2tex;

			unsigned int lightsUBO;

			static const int maxPointLights;// 8
			Basic::Ptr<PLDM_Generator> pldmGenerator;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_VIEWER_RASTER_BASE_H_
