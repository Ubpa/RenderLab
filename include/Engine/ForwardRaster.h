#pragma once

#include <Engine/Raster.h>

#include <OpenGL/VAO.h>
#include <OpenGL/Texture.h>

#include <UGM/transform.h>

namespace Ubpa {
	class Operation;
	class Image;

	class Camera;
	class RawAPI_OGLW;

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
		ForwardRaster(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera);
		virtual ~ForwardRaster() = default;

	public:
		virtual void Init() override;
		virtual void Draw() override;

	protected:// Use for SubClass
		void RegShader(Shader& shader, int depthmapBase = -1);
		void UseLightTex(const Shader& shader) const;

		void SetCurShader(const Shader& shader) { curShader = shader; }

	protected:
		virtual void Visit(Ptr<SObj> sobj);

		// SetCurShader for Primitive
		virtual void Visit(Ptr<BSDF_Diffuse> bsdf);
		virtual void Visit(Ptr<BSDF_Glass> bsdf);
		virtual void Visit(Ptr<BSDF_Mirror> bsdf);
		virtual void Visit(Ptr<BSDF_Emission> bsdf);
		virtual void Visit(Ptr<BSDF_CookTorrance> bsdf);
		virtual void Visit(Ptr<BSDF_MetalWorkflow> bsdf);
		virtual void Visit(Ptr<BSDF_FrostedGlass> bsdf);
		virtual void Visit(Ptr<BSDF_Frostbite> bsdf);
		virtual void Visit(Ptr<Gooch> gooch);

	protected:
		void SetDrawSky(bool drawSky) { this->drawSky = drawSky; }

	private: // no need to change
		/*virtual */void Visit(Ptr<Sphere> sphere);
		/*virtual */void Visit(Ptr<Plane> plane);
		/*virtual */void Visit(Ptr<TriMesh> mesh);

	private:
		void InitShaders();
		void InitShader_Basic();
		void InitShader_Skybox();

		// draw skybox as last
		void DrawEnvironment();

	private:
		Shader curShader;
		Shader shader_basic;
		Shader shader_skybox;

		std::vector<transformf> modelVec;

		std::map<WPtrC<Image>, Texture> img2tex;

		static const int maxPointLights;// 8
		static const int maxDirectionalLights;// 8
		static const int maxSpotLights;// 8

		struct ShaderCompare { bool operator()(const Shader& lhs, const Shader& rhs) const; };
		std::map<Shader, int, ShaderCompare> shader2depthmapBase;

		bool drawSky = true;
	};
}
