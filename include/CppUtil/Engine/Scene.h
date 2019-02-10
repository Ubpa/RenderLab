#ifndef _ENGINE_SCENE_H_
#define _ENGINE_SCENE_H_

#include <CppUtil/Basic/HeapObj.h>

#include <string>
#include <vector>

namespace CppUtil {
	namespace Engine {
		class Camera;
		class Light;

		class SObj;

		class Scene : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Scene)
		public:
			Scene(Basic::Ptr<SObj> rootSObj, const std::string & name = "");

			Basic::Ptr<SObj> GetRootSObj() const { return rootSObj; }
			Basic::Ptr<Camera> GetMainCamera() const { return mainCamera; }
			const std::vector<Basic::Ptr<Light>> & GetLights() const { return lights; }
			const std::vector<Basic::Ptr<SObj>> & GetGeoSObjs() const { return geoSObjs; }

		public:
			std::string name;

		private:
			Basic::Ptr<SObj> rootSObj;
			Basic::Ptr<Camera> mainCamera;
			std::vector<Basic::Ptr<Light>> lights;
			std::vector<Basic::Ptr<SObj>> geoSObjs;
		};
	}
}

#endif//!_ENGINE_SCENE_H_
