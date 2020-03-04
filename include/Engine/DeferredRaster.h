#ifndef _CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_
#define _CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_

#include <Engine/Raster.h>

#include <OpenGL/FBO.h>
#include <OpenGL/Texture.h>

#include <Basic/UGM/Transform.h>
#include <Basic/TypeMap.h>

#include <set>

namespace CppUtil{
	namespace Engine {
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
			void InitShader_GBuffer_MetalWorkflow();
			void InitShader_GBuffer_Diffuse();
			void InitShader_GBuffer_Frostbite();
			void InitShader_GBuffer_Emission();
			void InitShader_DirectLight();
			void InitShader_AmbientLight();
			void InitShader_Skybox();
			void InitShader_PostProcess();

		private:
			virtual void Visit(Basic::Ptr<SObj> sobj);

			virtual void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_Diffuse> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_Frostbite> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_Emission> emission);

			virtual void Visit(Basic::Ptr<Sphere> sphere);
			virtual void Visit(Basic::Ptr<Plane> plane);
			virtual void Visit(Basic::Ptr<TriMesh> mesh);
			virtual void Visit(Basic::Ptr<Disk> disk);
			virtual void Visit(Basic::Ptr<Capsule> capsule);

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
			OpenGL::Shader diffuseShader;
			OpenGL::Shader frostbiteShader;
			OpenGL::Shader emissionShader;

			OpenGL::Shader directLightShader;
			OpenGL::Shader ambientLightShader;
			OpenGL::Shader skyboxShader;
			OpenGL::Shader postProcessShader;

			OpenGL::Shader curMaterialShader;

			OpenGL::Texture ltcTex1; // GGX m(0,0) m(2,0) m(0,2) m(2,2)
			OpenGL::Texture ltcTex2; // GGX norm, fresnel, 0(unused), sphere

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
				Basic::TypeMap<int> idMap;
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
}

#endif // !_CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_
