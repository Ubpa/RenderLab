#pragma once
#ifndef _CPPUTIL_ENGINE_MESHEDIT_MIN_SURF_H_
#define _CPPUTIL_ENGINE_MESHEDIT_MIN_SURF_H_

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <Basic/UGM/UGM.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;
		class Paramaterize;

		class MinSurf : public Basic::HeapObj {
		public:
			MinSurf(Basic::Ptr<TriMesh> triMesh);
		public:
			static const Basic::Ptr<MinSurf> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<MinSurf>(triMesh);
			}
		public:
			void Clear();
			bool Init(Basic::Ptr<TriMesh> triMesh);

			bool Run();

		private:
			void Minimize();

		private:
			class V;
			class E;
			class P;
			class V : public Ubpa::TVertex<V, E, P> {
			public:
				Vec3 pos;
			};
			class E : public Ubpa::TEdge<V, E, P> { };
			class P :public Ubpa::TPolygon<V, E, P> { };
		private:
			friend class Paramaterize;

			Basic::Ptr<TriMesh> triMesh;
			const Basic::Ptr<Ubpa::HEMesh<V>> heMesh; // vertice order is same with triMesh
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MESHEDIT_MIN_SURF_H_
