#pragma once

#include <OpenGL/Shader.h>

#include <Basic/HeapObj.h>

#include <map>

namespace Ubpa {
	class RawAPI_OGLW;
	class Camera;

	class PLDM_Generator;
	class DLDM_Generator;
	class SLDM_Generator;
	class EnvGenerator;

	class Scene;

	class PointLight;
	class DirectionalLight;
	class SpotLight;
	class SphereLight;
	class DiskLight;
	class AreaLight;
	class CapsuleLight;

	/*
	光栅器
	*/
	class Raster : public HeapObj {
	protected:
		Raster(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera);
		virtual ~Raster() = default;

	public:
		// 改写后应调用该类 Init()
		virtual void Init();
		virtual void Resize() {}
		virtual void Draw() = 0;

	protected:
		// 更新各种光源的阴影贴图
		void UpdateShadowMap();

		// 更新环境贴图
		void UpdateEnvironment();

		// 更新光源的 UniformBlock
		void UpdateUBO();

		void BindUBO(Shader& shader);

	private:
		void UpdateUBO_PointLights();
		void UpdateUBO_DirectionalLights();
		void UpdateUBO_SpotLights();
		void UpdateUBO_Environment();
		void UpdateUBO_SphereLights();
		void UpdateUBO_DiskLights();
		void UpdateUBO_AreaLights();
		void UpdateUBO_CapsuleLights();

	protected:
		static const float lightNear;// 0.01
		static const float lightFar;// 25

		RawAPI_OGLW* pOGLW;
		Ptr<Scene> scene;
		Ptr<Camera> camera;

		Ptr<PLDM_Generator> pldmGenerator;
		Ptr<DLDM_Generator> dldmGenerator;
		Ptr<SLDM_Generator> sldmGenerator;
		Ptr<EnvGenerator> envGenerator;

		std::map<WPtrC<PointLight>, int> pointLight2idx;
		std::map<WPtrC<DirectionalLight>, int> directionalLight2idx;
		std::map<WPtrC<SpotLight>, int> spotLight2idx;
		std::map<WPtrC<SphereLight>, int> sphereLight2idx;
		std::map<WPtrC<DiskLight>, int> diskLight2idx;
		std::map<WPtrC<AreaLight>, int> areaLight2idx;
		std::map<WPtrC<CapsuleLight>, int> capsuleLight2idx;

	private:
		unsigned int pointLightsUBO;
		unsigned int directionalLightsUBO;
		unsigned int spotLightsUBO;
		unsigned int environmentUBO;
		unsigned int sphereLightsUBO;
		unsigned int diskLightsUBO;
		unsigned int areaLightsUBO;
		unsigned int capsuleLightsUBO;
	};
}
