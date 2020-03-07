#pragma once

#include <OpenGL/FBO.h>
#include <OpenGL/Texture.h>
#include <OpenGL/VAO.h>
#include <OpenGL/Shader.h>

#include <Basic/HeapObj.h>

#include <UGM/transform.h>

#include <UDP/Visitor.h>

namespace Ubpa {
	class Image;

	class RawAPI_OGLW;

	class Scene;
	class SObj;

	class Sphere;
	class Plane;
	class TriMesh;

	class InfiniteAreaLight;

	// Cubemap Generator, GPU-driven
	class EnvGenerator : public HeapObj {
	public:
		EnvGenerator(RawAPI_OGLW* pOGLW);

	public:
		static const Ptr<EnvGenerator> New(RawAPI_OGLW* pOGLW) {
			return Ubpa::New<EnvGenerator>(pOGLW);
		}

	protected:
		virtual ~EnvGenerator() = default;

	public:
		void Init();
		const Texture GetSkybox(PtrC<Image> img) const;
		const Texture GetIrradianceMap(PtrC<Image> img) const;
		const Texture GetPrefilterMap(PtrC<Image> img) const;
		const Texture GetBRDF_LUT() const { return brdfFBO.GetColorTexture(0); }

	public:
		void Visit(Ptr<Scene> scene);

	private:
		void InitFBOs();

		void InitShaders();
		void InitShader_genSkybox();
		void InitShader_genIrradiance();
		void InitShader_Prefilter();
		void InitShader_brdf();

		void InitBRDF_LUT();
		void UpdateTextures(PtrC<Image> img);
		void UpdateSkybox();
		void UpdateIrradianceMap();
		void UpdatePrefilterMap();

		void Clear();

	private:
		RawAPI_OGLW* pOGLW;

		int skyboxSize;
		int irradianceSize;
		int prefilterSize;
		int brdfSize;

		Shader shader_genSkybox;
		Shader shader_genIrradiance;
		Shader shader_prefilter;
		Shader shader_brdf;

		FBO genSkyboxFBO;
		FBO genIrradianceFBO;
		static constexpr int maxMipLevels = 5;
		FBO prefilterFBOs[maxMipLevels];
		bool isInitBrdfFBO{ false };
		FBO brdfFBO;

		WPtrC<Image> curImg;
		Texture skybox;
		Texture irradianceMap;
		Texture prefilterMap;

		static const transformf captureViews[6];
		static const FBO::TexTarget mapper[6];
	};
}
