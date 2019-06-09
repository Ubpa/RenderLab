#ifndef _ENGINE_VIEWER_VIEWER_H_
#define _ENGINE_VIEWER_VIEWER_H_

#include <CppUtil/Basic/HeapObj.h>
#include <enum.h>

namespace CppUtil {
	namespace QT {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class Scene;

		class Raster;
		class Roamer;
		class Picker;

		BETTER_ENUM(RasterType, int, DirectIllum, DeferredPipline, ForwardNPR);

		class Viewer final : public Basic::HeapObj {
		public:
			Viewer(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, RasterType rasterType);

		public:
			static const Basic::Ptr<Viewer> New(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, RasterType rasterType) {
				return Basic::New<Viewer>(pOGLW, scene, rasterType);
			}

		protected:
			virtual ~Viewer() = default;

		public:
			void SetRaster(RasterType rasterType);

		public:
			QT::RawAPI_OGLW * GetOGLW() const { return pOGLW; }
			Basic::Ptr<Scene> GetScene() const { return scene; }
			Basic::Ptr<Raster> GetRaster() const { return raster; }
			Basic::Ptr<Roamer> GetRoamer() const { return roamer; }
			Basic::Ptr<Picker> GetPicker() const { return picker; }

		public:
			void SetLock(bool isLock);

		private:
			QT::RawAPI_OGLW * pOGLW;
			Basic::Ptr<Scene> scene;
			Basic::Ptr<Raster> raster;
			Basic::Ptr<Roamer> roamer;
			Basic::Ptr<Picker> picker;
		};
	}
}

#endif//!_ENGINE_VIEWER_VIEWER_H_
