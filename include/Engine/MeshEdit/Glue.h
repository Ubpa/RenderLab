#pragma once
#ifndef _CPPUTIL_ENGINE_MESHEDIT_GLUE_H_
#define _CPPUTIL_ENGINE_MESHEDIT_GLUE_H_

#include <Basic/HeapObj.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;
		class Glue : public Basic::HeapObj {
		public:
			Glue(Basic::Ptr<TriMesh> triMesh) { Init(triMesh); }

		public:
			static const Basic::Ptr<Glue> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<Glue>(triMesh);
			}

		public:
			void Init(Basic::Ptr<TriMesh> triMesh) { this->triMesh = triMesh; }
			void Clear() { triMesh = nullptr; }
			bool Run();

		private:
			Basic::Ptr<TriMesh> triMesh;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MESHEDIT_GLUE_H_
