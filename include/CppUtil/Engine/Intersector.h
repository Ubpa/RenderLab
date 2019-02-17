#ifndef _ENGINE_INTERSECTOR_INTERSECTOR_H_
#define _ENGINE_INTERSECTOR_INTERSECTOR_H_

#include <CppUtil/Basic/EleVisitor.h>

namespace CppUtil {
	namespace Engine {
		class Intersector : public Basic::EleVisitor {
			ELEVISITOR_SETUP(Intersector)
		public:
			struct Rst {
				Rst(bool isIntersect = false)
					: isIntersect(false) { }

			protected:
				bool isIntersect;
			};
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_INTERSECTOR_H_