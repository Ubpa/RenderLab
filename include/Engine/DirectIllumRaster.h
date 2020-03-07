#pragma once

#include <Engine/ForwardRaster.h>

namespace Ubpa {
	class DirectIllumRaster : public ForwardRaster {
	public:
		DirectIllumRaster(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
			: ForwardRaster(pOGLW, scene, camera) { }

	public:
		static const Ptr<DirectIllumRaster> New(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera) {
			return Ubpa::New<DirectIllumRaster>(pOGLW, scene, camera);
		}

	protected:
		virtual ~DirectIllumRaster() = default;

	public:
		virtual void Init() override;

	protected:
		virtual void ImplVisit(Ptr<BSDF_Diffuse> bsdf) override;
		virtual void ImplVisit(Ptr<BSDF_MetalWorkflow> bsdf) override;
		virtual void ImplVisit(Ptr<BSDF_FrostedGlass> bsdf) override;

	private:
		void InitShaders();
		void InitShaderDiffuse();
		void InitShaderMetalWorkflow();
		void InitShaderFrostedGlass();

	private:
		Shader shader_diffuse;
		Shader shader_metalWorkflow;
		Shader shader_frostedGlass;
	};
}
