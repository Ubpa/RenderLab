#pragma once

#include <Engine/Viewer/ForwardRaster.h>

namespace Ubpa {
	class WireframeRaster : public ForwardRaster {
	public:
		WireframeRaster(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
			: ForwardRaster(pOGLW, scene, camera) { }

	public:
		static Ptr<WireframeRaster> New(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, Ptr<Camera> camera) {
			return Ubpa::New<WireframeRaster>(pOGLW, scene, camera);
		}

	protected:
		virtual ~WireframeRaster() = default;

	public:
		virtual void Init() override;
	};
}
