#ifndef _INTERSECTOR_H_
#define _INTERSECTOR_H_

#include <CppUtil/Basic/EleVisitor.h>

namespace CppUtil {
	namespace Engine {
		// list out all primitives
		class Sphere;

		class Intersector : public Basic::EleVisitor {
			ELEVISITOR_SETUP(Intersector)
		public:
			struct Rst {
				Rst(bool isIntersect = false)
					: isIntersect(false) { }

				bool isIntersect;
			};

		public:
			virtual const Rst & GetRst() const = 0;
		};
	}
}

#endif//!_INTERSECTOR_H_