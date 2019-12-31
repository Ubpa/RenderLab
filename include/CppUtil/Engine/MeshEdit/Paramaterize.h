#pragma once
#ifndef _CPPUTIL_ENGINE_MESHEDIT_PARAMATERIZE_H_
#define _CPPUTIL_ENGINE_MESHEDIT_PARAMATERIZE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/HEMesh/HEMesh.h>
#include <CppUtil/Basic/UGM/UGM.h>

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

#endif // !_CPPUTIL_ENGINE_MESHEDIT_PARAMATERIZE_H_
