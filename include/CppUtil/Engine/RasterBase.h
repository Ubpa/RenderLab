#ifndef _CPPUTIL_ENGINE_VIEWER_RASTER_BASE_H_
#define _CPPUTIL_ENGINE_VIEWER_RASTER_BASE_H_

#include <CppUtil/Basic/Visitor.h>

#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/OpenGL/Texture.h>

#include <CppUtil/Basic/UGM/Transform.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
		class Image;
	}

	namespace OpenGL {
		class Camera;
	}

	namespace QT {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class PLDM_Generator;
		class DLDM_Generator;
		class SLDM_Generator;

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

		class PointLight;
		class DirectionalLight;
		class SpotLight;

		class RasterBase : public Basic::Visitor {
		protected:
			RasterBase(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera);
			virtual ~RasterBase() = default;

		public:
			virtual void Draw();

			virtual void OGL_Init();

		protected:// Use for SubClass
			void RegShader(const OpenGL::Shader & shader, int depthmapBase = -1);
			void UseLightDepthMap(const OpenGL::Shader & shader) const;

			void SetCurShader(const OpenGL::Shader & shader) { curShader = shader; }

		protected:
			virtual void Visit(Basic::Ptr<SObj> sobj);

			// SetCurShader for Primitive
			virtual void Visit(Basic::Ptr<BSDF_Diffuse> bsdf) { curShader = shader_basic; }
			virtual void Visit(Basic::Ptr<BSDF_Glass> bsdf) { curShader = shader_basic; }
			virtual void Visit(Basic::Ptr<BSDF_Mirror> bsdf) { curShader = shader_basic; }
			virtual void Visit(Basic::Ptr<BSDF_Emission> bsdf) { curShader = shader_basic; }
			virtual void Visit(Basic::Ptr<BSDF_CookTorrance> bsdf) { curShader = shader_basic; }
			virtual void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf) { curShader = shader_basic; }
			virtual void Visit(Basic::Ptr<BSDF_FrostedGlass> bsdf) { curShader = shader_basic; }

		private: // no need to change
			/*virtual */void Visit(Basic::Ptr<Sphere> sphere);
			/*virtual */void Visit(Basic::Ptr<Plane> plane);
			/*virtual */void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			void InitShaders();
			void InitShader_Basic();

			// 更新光源的 UniformBlock
			void UpdateLights();
			void UpdatePointLights();
			void UpdateDirectionalLights();
			void UpdateSpotLights();

		protected:
			Basic::Ptr<Scene> scene;
			QT::RawAPI_OGLW * pOGLW;

		private:
			OpenGL::Shader curShader;
			OpenGL::Shader shader_basic;

			std::vector<Transform> modelVec;


			std::map<Basic::WPtrC<Basic::Image>, OpenGL::Texture> img2tex;

			unsigned int pointLightsUBO;
			unsigned int directionalLightsUBO;
			unsigned int spotLightsUBO;

			static const int maxPointLights;// 8
			Basic::Ptr<PLDM_Generator> pldmGenerator;
			static const int maxDirectionalLights;// 8
			Basic::Ptr<DLDM_Generator> dldmGenerator;
			static const int maxSpotLights;// 8
			Basic::Ptr<SLDM_Generator> sldmGenerator;

			static const float lightNear;// 0.01
			static const float lightFar;// 25

			struct ShaderCompare { bool operator()(const OpenGL::Shader & lhs, const OpenGL::Shader & rhs) const; };
			std::map<OpenGL::Shader, int, ShaderCompare> shader2depthmapBase;
			std::map<Basic::WPtrC<PointLight>, int> pointLight2idx;
			std::map<Basic::WPtrC<DirectionalLight>, int> directionalLight2idx;
			std::map<Basic::WPtrC<SpotLight>, int> spotLight2idx;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_VIEWER_RASTER_BASE_H_
