#ifndef _ENGINE_VIEWER_PICKER_H_
#define _ENGINE_VIEWER_PICKER_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Engine {
		class Viewer;

		class RayIntersector;

		class Picker : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Picker)
		public:
			Picker(Viewer * viewer);

			void Init();

			Viewer * GetViewer() const { return viewer; }
		private:
			Viewer * viewer;

			Basic::Ptr<RayIntersector> rayIntersector;
		};
	}
}

#endif//!_ENGINE_VIEWER_PICKER_H_
