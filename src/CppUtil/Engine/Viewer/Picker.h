#ifndef _ENGINE_VIEWER_PICKER_H_
#define _ENGINE_VIEWER_PICKER_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Engine {
		class Viewer;

		class RayIntersector;

		class Picker final : public Basic::HeapObj {
		public:
			Picker(Viewer * viewer);

		public:
			static const Basic::Ptr<Picker> New(Viewer * viewer) {
				return Basic::New<Picker>(viewer);
			}

		protected:
			virtual ~Picker() = default;

		public:
			void Init();

			Viewer * GetViewer() const { return viewer; }

		private:
			Viewer * viewer;

			Basic::Ptr<RayIntersector> rayIntersector;
		};
	}
}

#endif//!_ENGINE_VIEWER_PICKER_H_
