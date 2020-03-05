#pragma once

#include <Engine/Raster.h>

#include <OpenGL/VAO.h>
#include <OpenGL/Texture.h>

#include <UGM/transform.h>

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
		class BSDF_Frostbite;
		class Gooch;

		class ForwardRaster : public Raster {
		protected:
			ForwardRaster(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera);
			virtual ~ForwardRaster() = default;

		public:
			virtual void Init() override;
			virtual void Draw() override;

		protected:// Use for SubClass
			void RegShader(OpenGL::Shader & shader, int depthmapBase = -1);
			void UseLightTex(const OpenGL::Shader & shader) const;

			void SetCurShader(const OpenGL::Shader & shader) { curShader = shader; }

		protected:
			virtual void Visit(Basic::Ptr<SObj> sobj);

			// SetCurShader for Primitive
			virtual void Visit(Basic::Ptr<BSDF_Diffuse> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_Glass> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_Mirror> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_Emission> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_CookTorrance> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_FrostedGlass> bsdf);
			virtual void Visit(Basic::Ptr<BSDF_Frostbite> bsdf);
			virtual void Visit(Basic::Ptr<Gooch> gooch);

		protected:
			void SetDrawSky(bool drawSky) { this->drawSky = drawSky; }

		private: // no need to change
			/*virtual */void Visit(Basic::Ptr<Sphere> sphere);
			/*virtual */void Visit(Basic::Ptr<Plane> plane);
			/*virtual */void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			void InitShaders();
			void InitShader_Basic();
			void InitShader_Skybox();

			// draw skybox as last
			void DrawEnvironment();

		private:
			OpenGL::Shader curShader;
			OpenGL::Shader shader_basic;
			OpenGL::Shader shader_skybox;

			std::vector<Ubpa::transformf> modelVec;

			std::map<Basic::WPtrC<Basic::Image>, OpenGL::Texture> img2tex;

			static const int maxPointLights;// 8
			static const int maxDirectionalLights;// 8
			static const int maxSpotLights;// 8

			struct ShaderCompare { bool operator()(const OpenGL::Shader & lhs, const OpenGL::Shader & rhs) const; };
			std::map<OpenGL::Shader, int, ShaderCompare> shader2depthmapBase;

			bool drawSky = true;
		};
	}
}
