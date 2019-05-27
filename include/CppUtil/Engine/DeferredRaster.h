#ifndef _CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_
#define _CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_

#include <CppUtil/Engine/Raster.h>

#include <CppUtil/OpenGL/FBO.h>
#include <CppUtil/Basic/UGM/Transform.h>

namespace CppUtil{
	namespace Engine {
		class SObj;

		class Sphere;
		class Plane;
		class TriMesh;

		class BSDF_MetalWorkflow;

		/*
		延迟管线
		暂时只用于金属工作流，当需求来的时候，再来搞一个公共的基类
		*/
		class DeferredRaster : public Raster {
		public:
			DeferredRaster(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera);

		public:
			static const Basic::Ptr<DeferredRaster> New(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera) {
				return Basic::New<DeferredRaster>(pOGLW, scene, camera);
			}

		public:
			virtual void Init() override;
			virtual void Resize() override;
			virtual void Draw() override;

		private:
			void InitShaders();
			void InitShader_GBuffer();
			void InitShader_DirectLight();
			void InitShader_AmbientLight();
			void InitShader_Skybox();
			void InitShader_PostProcess();

		private:
			virtual void Visit(Basic::Ptr<SObj> sobj);

			virtual void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf);

			virtual void Visit(Basic::Ptr<Sphere> sphere);
			virtual void Visit(Basic::Ptr<Plane> plane);
			virtual void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			void Pass_GBuffer();
			void Pass_DirectLight();
			void Pass_AmbientLight();
			void Pass_Skybox();
			void Pass_PostProcess();

		private:
			OpenGL::FBO gbufferFBO;
			OpenGL::FBO windowFBO;

			std::vector<Transform> modelVec;
			OpenGL::Shader metalShader;
			OpenGL::Shader directLightShader;
			OpenGL::Shader ambientLightShader;
			OpenGL::Shader skyboxShader;
			OpenGL::Shader postProcessShader;

			static const int maxPointLights;// 8
			static const int maxDirectionalLights;// 8
			static const int maxSpotLights;// 8
		};
	}
}

#endif // !_CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_
