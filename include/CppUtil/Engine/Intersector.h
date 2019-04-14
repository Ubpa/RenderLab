#ifndef _ENGINE_INTERSECTOR_INTERSECTOR_H_
#define _ENGINE_INTERSECTOR_INTERSECTOR_H_

#include <CppUtil/Basic/Visitor.h>

namespace CppUtil {
	namespace Engine {
		class Intersector : public Basic::Visitor {
		protected:
			Intersector() = default;
			virtual ~Intersector() = default;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_INTERSECTOR_H_