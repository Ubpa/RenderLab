#ifndef _ENGINE_VIEWER_PICKER_H_
#define _ENGINE_VIEWER_PICKER_H_

#include <CppUtil/Basic/HeapObj.h>
#include <qobject.h>

namespace Ui{
	class Attribute;
}

namespace CppUtil {
	namespace OpenGL {
		class Camera;
	}

	namespace Qt {
		class RawAPI_OGLW;
	}

	namespace Engine {
		class Viewer;
		class SObj;

		class Ray;
		class RayIntersector;

		class Picker : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Picker)
		public:
			Picker(Viewer * viewer);

			void Init();

			Viewer * GetViewer() const { return viewer; }
		private:
			Viewer * viewer;

			Basic::Ptr<Ray> ray;
			Basic::Ptr<RayIntersector> rayIntersector;
		};
	}
}

#endif//!_ENGINE_VIEWER_PICKER_H_
