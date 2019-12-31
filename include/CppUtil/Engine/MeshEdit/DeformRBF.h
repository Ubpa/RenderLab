#pragma once
#ifndef _CPPUTIL_ENGINE_MESHEDIT_DEFORMRBF_H_
#define _CPPUTIL_ENGINE_MESHEDIT_DEFORMRBF_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Basic/UGM/UGM.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;
		class DeformRBF : public Basic::HeapObj {
		public:
			struct Constraint {
				size_t id; // vertex id in triMesh
				Point3 pos; // target position
			};

		public:
			DeformRBF(Basic::Ptr<TriMesh> triMesh) { Init(triMesh); }
		public:
			static const Basic::Ptr<DeformRBF> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<DeformRBF>(triMesh);
			}

		public:
			void Clear();
			bool Init(Basic::Ptr<TriMesh> triMesh);
			bool Run(const std::vector<Constraint> & constraints);
			void Restore();
			 
		private:
			Basic::Ptr<TriMesh> triMesh;
			std::vector<Point3> origPos;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MESHEDIT_DEFORMRBF_H_
