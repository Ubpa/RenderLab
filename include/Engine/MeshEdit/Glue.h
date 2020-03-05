#pragma once

#include <Basic/HeapObj.h>

namespace Ubpa {
	class TriMesh;
	class Glue : public HeapObj {
	public:
		Glue(Ptr<TriMesh> triMesh) { Init(triMesh); }

	public:
		static const Ptr<Glue> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<Glue>(triMesh);
		}

	public:
		void Init(Ptr<TriMesh> triMesh) { this->triMesh = triMesh; }
		void Clear() { triMesh = nullptr; }
		bool Run();

	private:
		Ptr<TriMesh> triMesh;
	};
}
