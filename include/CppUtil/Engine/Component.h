#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class Component : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Component)
		public:
			Basic::Ptr<SObj> GetSObj() { return sobj; }
		private:
			Basic::Ptr<SObj> sobj;
		};
	}
}

#endif // !_COMPONENT_H_
