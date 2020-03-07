#pragma once

#include <Basic/HeapObj.h>

namespace Ubpa {
	class RawAPI_OGLW;

	class Scene;

	class Raster;
	class Roamer;
	class Picker;

	class Viewer final : public HeapObj {
	public:
		enum class RasterType {
			DirectIllum, DeferredPipeline, ForwardNPR, WireframeRaster
		};

		Viewer(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, RasterType rasterType);

	public:
		static const Ptr<Viewer> New(RawAPI_OGLW* pOGLW, Ptr<Scene> scene, RasterType rasterType) {
			return Ubpa::New<Viewer>(pOGLW, scene, rasterType);
		}

	protected:
		virtual ~Viewer() = default;

	public:
		void SetRaster(RasterType rasterType);

	public:
		RawAPI_OGLW* GetOGLW() const { return pOGLW; }
		Ptr<Scene> GetScene() const { return scene; }
		Ptr<Raster> GetRaster() const { return raster; }
		Ptr<Roamer> GetRoamer() const { return roamer; }
		Ptr<Picker> GetPicker() const { return picker; }

	public:
		void SetLock(bool isLock);

	private:
		RawAPI_OGLW* pOGLW;
		Ptr<Scene> scene;
		Ptr<Raster> raster;
		Ptr<Roamer> roamer;
		Ptr<Picker> picker;
	};
}
