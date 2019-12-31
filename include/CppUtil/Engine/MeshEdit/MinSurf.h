#pragma once
#ifndef _CPPUTIL_ENGINE_MESHEDIT_MIN_SURF_H_
#define _CPPUTIL_ENGINE_MESHEDIT_MIN_SURF_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/HEMesh/HEMesh.h>
#include <CppUtil/Basic/UGM/UGM.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;

		class MinSurf : public Basic::HeapObj {
		public:
			MinSurf(Basic::Ptr<TriMesh> triMesh) { Init(triMesh); }
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
			class V : public Basic::TVertex<V, E, P> {
			public:
				Vec3 pos;
			};
			class E : public Basic::TEdge<V, E, P> { };
			class P :public Basic::TPolygon<V, E, P> { };
		private:
			Basic::Ptr<TriMesh> triMesh;
			Basic::Ptr<Basic::HEMesh<V>> heMesh;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MESHEDIT_MIN_SURF_H_
