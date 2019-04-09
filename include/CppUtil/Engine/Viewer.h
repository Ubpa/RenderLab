#ifndef _ENGINE_VIEWER_VIEWER_H_
#define _ENGINE_VIEWER_VIEWER_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace QT {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class Scene;

		class DirectIllumRaster;
		class Roamer;
		class Picker;

		class Viewer final : public Basic::HeapObj {
		public:
			Viewer(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene);

		public:
			static const Basic::Ptr<Viewer> New(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene) {
				return Basic::New<Viewer>(pOGLW, scene);
			}

		protected:
			virtual ~Viewer() = default;

		public:
			QT::RawAPI_OGLW * GetOGLW() const { return pOGLW; }
			Basic::Ptr<Scene> GetScene() const { return scene; }
			Basic::Ptr<DirectIllumRaster> GetRaster() const { return raster; }
			Basic::Ptr<Roamer> GetRoamer() const { return roamer; }
			Basic::Ptr<Picker> GetPicker() const { return picker; }

		public:
			void SetLock(bool isLock);

		private:
			QT::RawAPI_OGLW * pOGLW;
			Basic::Ptr<Picker> picker;
			Basic::Ptr<Scene> scene;
			Basic::Ptr<DirectIllumRaster> raster;
			Basic::Ptr<Roamer> roamer;
		};
	}
}

#endif//!_ENGINE_VIEWER_VIEWER_H_
