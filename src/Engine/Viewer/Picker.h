#pragma once

#include <Basic/HeapObj.h>
#include <Engine/MeshEdit/DeformRBF.h>
#include <UGM/UGM>

namespace Ubpa {
	class Viewer;

	class ClosestIntersector;

	class Picker final : public HeapObj {
	public:
		Picker(Viewer* viewer);

	public:
		static const Ptr<Picker> New(Viewer* viewer) {
			return Ubpa::New<Picker>(viewer);
		}

	protected:
		virtual ~Picker() = default;

	public:
		void Init();

		Viewer* GetViewer() const { return viewer; }

	private:
		Viewer* viewer;

		Ptr<ClosestIntersector> closestIntersector;
		Ptr<DeformRBF> deformRBF;
		std::vector<DeformRBF::Constraint> cons;
		std::vector<size_t> deformIndice;
		bool isInDeform = false;
		pointf3 origPos;
		vecf3 dir;
		int y;
		int lastY;
	};
}
