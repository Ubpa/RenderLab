#ifndef _CPPUTIL_ENGINE_VIEWER_FORWARD_RASTER_H_
#define _CPPUTIL_ENGINE_VIEWER_FORWARD_RASTER_H_

#include <CppUtil/Engine/Raster.h>

#include <CppUtil/OpenGL/VAO.h>
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

		class ForwardRaster : public Raster {
		protected:
			ForwardRaster(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera);
			virtual ~ForwardRaster() = default;

		public:
			virtual void Draw();

			virtual void Init();

		protected:// Use for SubClass
			void RegShader(OpenGL::Shader & shader, int depthmapBase = -1);
			void UseLightTex(const OpenGL::Shader & shader) const;

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
			void InitShader_Skybox();

			// draw skybox as last
			void DrawEnvironment();

		private:
			OpenGL::Shader curShader;
			OpenGL::Shader shader_basic;
			OpenGL::Shader shader_skybox;

			std::vector<Transform> modelVec;

			std::map<Basic::WPtrC<Basic::Image>, OpenGL::Texture> img2tex;

			static const int maxPointLights;// 8
			static const int maxDirectionalLights;// 8
			static const int maxSpotLights;// 8

			struct ShaderCompare { bool operator()(const OpenGL::Shader & lhs, const OpenGL::Shader & rhs) const; };
			std::map<OpenGL::Shader, int, ShaderCompare> shader2depthmapBase;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_VIEWER_FORWARD_RASTER_H_
