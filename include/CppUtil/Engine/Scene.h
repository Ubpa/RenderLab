#ifndef _ENGINE_SCENE_H_
#define _ENGINE_SCENE_H_

#include <CppUtil/Basic/HeapObj.h>

#include <string>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class Scene : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Scene)
		public:
			Scene(Basic::Ptr<SObj> rootSObj, const std::string & name = "")
				:rootSObj(rootSObj), name(name) { }

			Basic::Ptr<SObj> GetRootSObj() const { return rootSObj; }

		private:
			Basic::Ptr<SObj> rootSObj;

		public:
			std::string name;
		};
	}
}

#endif//!_ENGINE_SCENE_H_
