#ifndef _INTERSECTOR_H_
#define _INTERSECTOR_H_

#include <CppUtil/Basic/EleVisitor.h>

namespace CppUtil {
	namespace Engine {
		// list out all primitives
		class Sphere;

		class Intersector : public Basic::EleVisitor {
		public:
			struct Rst {
				class Info { };

				bool isIntersect;
				Basic::Ptr<Info> info;
			};

		public:
			virtual Rst Intersect(Basic::CPtr<Sphere> sphere) = 0;
		};
	}
}

#endif//!_INTERSECTOR_H_