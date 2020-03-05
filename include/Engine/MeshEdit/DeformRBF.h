#pragma once

#include <Basic/HeapObj.h>
#include <Engine/TriMesh.h>
#include <UGM/UGM>

namespace CppUtil {
	namespace Engine {
		class TriMesh;
		class DeformRBF : public Basic::HeapObj {
		public:
			struct Constraint {
				size_t id; // vertex id in triMesh
				Ubpa::pointf3 pos; // target position
			};

		public:
			DeformRBF(Basic::Ptr<TriMesh> triMesh) { Init(triMesh); }
		public:
			static const Basic::Ptr<DeformRBF> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<DeformRBF>(triMesh);
			}
		public:
			const Basic::Ptr<TriMesh> GetMesh() const { return triMesh; }

		public:
			void Clear();
			bool Init(Basic::Ptr<TriMesh> triMesh);
			bool Run(const std::vector<Constraint> & constraints, const std::vector<size_t> & updateIndice);
			void Restore();
			 
		private:
			Basic::Ptr<TriMesh> triMesh;
			std::vector<Ubpa::pointf3> origPos;
		};
	}
}
