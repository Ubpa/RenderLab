#pragma once

#include <Engine/Viewer/ForwardRaster.h>

namespace Ubpa {
	class ForwardNPR : public ForwardRaster {
	public:
		ForwardNPR(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
			: ForwardRaster(pOGLW, scene, camera) { }

	public:
		static Ptr<ForwardNPR> New(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera) {
			return Ubpa::New<ForwardNPR>(pOGLW, scene, camera);
		}

	protected:
		virtual ~ForwardNPR() = default;

	public:
		virtual void Init() override;

	protected:
		virtual void ImplVisit(Ptr<Gooch> gooch) override;

	private:
		void InitShaders();
		void InitShaderGooch();

	private:
		Shader shaderGooch;
	};
}
