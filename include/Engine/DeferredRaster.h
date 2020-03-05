#pragma once

#include <Engine/Raster.h>

#include <OpenGL/FBO.h>
#include <OpenGL/Texture.h>

#include <UGM/transform.h>
#include <Basic/TypeMap.h>

#include <set>

namespace Ubpa {
	class SObj;

	class Sphere;
	class Plane;
	class TriMesh;
	class Disk;
	class Capsule;

	class Material;
	class BSDF_MetalWorkflow;
	class BSDF_Diffuse;
	class BSDF_Frostbite;
	class BSDF_Emission;

	class DeferredRaster : public Raster {
	public:
		DeferredRaster(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera);

	public:
		static const Ptr<DeferredRaster> New(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera) {
			return Ubpa::New<DeferredRaster>(pOGLW, scene, camera);
		}

	public:
		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;

	private:
		void InitShaders();
		void InitShader_GBuffer();
		void InitShader_GBuffer_MetalWorkflow();
		void InitShader_GBuffer_Diffuse();
		void InitShader_GBuffer_Frostbite();
		void InitShader_GBuffer_Emission();
		void InitShader_DirectLight();
		void InitShader_AmbientLight();
		void InitShader_Skybox();
		void InitShader_PostProcess();

	private:
		virtual void Visit(Ptr<SObj> sobj);

		virtual void Visit(Ptr<BSDF_MetalWorkflow> bsdf);
		virtual void Visit(Ptr<BSDF_Diffuse> bsdf);
		virtual void Visit(Ptr<BSDF_Frostbite> bsdf);
		virtual void Visit(Ptr<BSDF_Emission> emission);

		virtual void Visit(Ptr<Sphere> sphere);
		virtual void Visit(Ptr<Plane> plane);
		virtual void Visit(Ptr<TriMesh> mesh);
		virtual void Visit(Ptr<Disk> disk);
		virtual void Visit(Ptr<Capsule> capsule);

	private:
		void Pass_GBuffer();
		void Pass_DirectLight();
		void Pass_AmbientLight();
		void Pass_Skybox();
		void Pass_PostProcess();

	private:
		FBO gbufferFBO;
		FBO windowFBO;

		std::vector<transformf> modelVec;

		Shader metalShader;
		Shader diffuseShader;
		Shader frostbiteShader;
		Shader emissionShader;

		Shader directLightShader;
		Shader ambientLightShader;
		Shader skyboxShader;
		Shader postProcessShader;

		Shader curMaterialShader;

		Texture ltcTex1; // GGX m(0,0) m(2,0) m(0,2) m(2,2)
		Texture ltcTex2; // GGX norm, fresnel, 0(unused), sphere

		static const int maxPointLights;// 8
		static const int maxDirectionalLights;// 8
		static const int maxSpotLights;// 8

		// manager material ID
		class MngrMID {
		public:
			// ID >= 0
			template<typename T, typename = std::enable_if_t<std::is_base_of_v<Material, T>>>
			inline void Reg(int ID);
			template<typename T, typename = std::enable_if_t<std::is_base_of_v<Material, T>>>
			inline int Get() const;
		private:
			TypeMap<int> idMap;
			std::set<int> registedID;
		};
		MngrMID mngrMID;
	};

	template<typename T, typename>
	int DeferredRaster::MngrMID::Get() const {
		auto target = idMap.find(typeid(T));
		if (target == idMap.cend()) {
			printf("ERROR::DeferredRaster::Get:\n"
				"\ttype[%s] is not registed\n", typeid(T).name());
			return -1;
		}
		return target->second;
	}

	template<typename T, typename>
	void DeferredRaster::MngrMID::Reg(int ID) {
		auto target = idMap.find(typeid(T));
		if (target != idMap.end()) {
			printf("WARNNING::DeferredRaster::Reg:\n"
				"\t""type(%s) is already registed\n", typeid(T).name());
			return;
		}
		if (registedID.find(ID) != registedID.end()) {
			printf("ERROR::DeferredRaster::Reg:\n"
				"\t""ID(%d) is already registed\n", ID);
			return;
		}

		idMap[typeid(T)] = ID;
		registedID.insert(ID);
	}
}
