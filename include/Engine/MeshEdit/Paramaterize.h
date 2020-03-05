#pragma once

#include <Basic/HeapObj.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;
		class MinSurf;

		// mesh boundary == 1
		class Paramaterize : public Basic::HeapObj {
		public:
			Paramaterize(Basic::Ptr<TriMesh> triMesh);
		public:
			static const Basic::Ptr<Paramaterize> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<Paramaterize>(triMesh);
			}
		public:
			void Clear();
			bool Init(Basic::Ptr<TriMesh> triMesh);

			// need init again after run
			bool Run();

		private:
			void SetBoundPos();

		private:
			const Basic::Ptr<MinSurf> minSurf;
		};
	}
}
