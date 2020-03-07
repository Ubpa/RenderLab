#pragma once

#include <Basic/HeapObj.h>
#include <Engine/Primitive/TriMesh.h>
#include <UGM/UGM>

namespace Ubpa {
	class TriMesh;
	class DeformRBF : public HeapObj {
	public:
		struct Constraint {
			size_t id; // vertex id in triMesh
			pointf3 pos; // target position
		};

	public:
		DeformRBF(Ptr<TriMesh> triMesh) { Init(triMesh); }
	public:
		static const Ptr<DeformRBF> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<DeformRBF>(triMesh);
		}
	public:
		const Ptr<TriMesh> GetMesh() const { return triMesh; }

	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		bool Run(const std::vector<Constraint>& constraints, const std::vector<size_t>& updateIndice);
		void Restore();

	private:
		Ptr<TriMesh> triMesh;
		std::vector<pointf3> origPos;
	};
}
