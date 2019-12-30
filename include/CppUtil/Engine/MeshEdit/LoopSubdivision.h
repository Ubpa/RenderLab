#pragma once
#ifndef _CPPUTIL_ENGINE_MESHEDIT_LOOP_SUBDIVISION_H_
#define _CPPUTIL_ENGINE_MESHEDIT_LOOP_SUBDIVISION_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/HEMesh/HEMesh.h>
#include <CppUtil/Basic/UGM/UGM.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;

		class LoopSubdivision : public Basic::HeapObj {
		public:
			LoopSubdivision(Basic::Ptr<TriMesh> triMesh) { Init(triMesh); }

		public:
			static const Basic::Ptr<LoopSubdivision> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<LoopSubdivision>(triMesh);
			}

		public:
			bool Init(Basic::Ptr<TriMesh> triMesh);
			void Clear();
			bool Run(size_t n);

		private:
			void HEMeshSubdivision();

		private:
			class V;
			class E;
			class P;
			class V : public Basic::TVertex<V, E, P> {
			public:
				Vec3 pos;
				bool isNew = false;
				Vec3 newPos;
			};
			class E : public Basic::TEdge<V, E, P> {
			public:
				bool isNew = false;
				Vec3 newPos;
			};
			class P :public Basic::TPolygon<V, E, P> { };
		private:
			Basic::Ptr<TriMesh> triMesh;
			Basic::Ptr<Basic::HEMesh<V>> heMesh;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MESHEDIT_LOOP_SUBDIVISION_H_
