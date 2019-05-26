#ifndef _CPPUTIL_ENGINE_VIEWER_RASTER_H_
#define _CPPUTIL_ENGINE_VIEWER_RASTER_H_

#include <CppUtil/OpenGL/Shader.h>

#include <CppUtil/Basic/Visitor.h>

#include <map>

namespace CppUtil {
	namespace QT {
		class RawAPI_OGLW;
	}

	namespace OpenGL {
		class Camera;
	}

	namespace Engine {
		class PLDM_Generator;
		class DLDM_Generator;
		class SLDM_Generator;
		class EnvGenerator;

		class Scene;

		class PointLight;
		class DirectionalLight;
		class SpotLight;

		/*
		光栅器
		*/
		class Raster : public Basic::Visitor {
		protected:
			Raster(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera);
			virtual ~Raster() = default;

		public:
			// 改写后应调用该类 Init()
			virtual void Init();
			virtual void Draw() = 0;

		protected:
			// 更新各种光源的阴影贴图
			void UpdateShadowMap();
			
			// 更新环境贴图
			void UpdateEnvironment();

			// 更新光源的 UniformBlock
			void UpdateUBO();

			void BindUBO(OpenGL::Shader & shader);

		private:
			void UpdateUBO_PointLights();
			void UpdateUBO_DirectionalLights();
			void UpdateUBO_SpotLights();
			void UpdateUBO_Environment();

		protected:
			static const float lightNear;// 0.01
			static const float lightFar;// 25

			QT::RawAPI_OGLW * pOGLW;
			Basic::Ptr<Scene> scene;
			Basic::Ptr<OpenGL::Camera> camera;

			Basic::Ptr<PLDM_Generator> pldmGenerator;
			Basic::Ptr<DLDM_Generator> dldmGenerator;
			Basic::Ptr<SLDM_Generator> sldmGenerator;
			Basic::Ptr<EnvGenerator> envGenerator;

			std::map<Basic::WPtrC<PointLight>, int> pointLight2idx;
			std::map<Basic::WPtrC<DirectionalLight>, int> directionalLight2idx;
			std::map<Basic::WPtrC<SpotLight>, int> spotLight2idx;

		private:
			unsigned int pointLightsUBO;
			unsigned int directionalLightsUBO;
			unsigned int spotLightsUBO;
			unsigned int environmentUBO;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_VIEWER_RASTER_H_