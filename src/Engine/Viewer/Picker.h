#pragma once

#include <Basic/HeapObj.h>
#include <Engine/MeshEdit/DeformRBF.h>
#include <UGM/UGM>

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
			Basic::Ptr<DeformRBF> deformRBF;
			std::vector<DeformRBF::Constraint> cons;
			std::vector<size_t> deformIndice;
			bool isInDeform = false;
			Ubpa::pointf3 origPos;
			Ubpa::vecf3 dir;
			int y;
			int lastY;
		};
	}
}
