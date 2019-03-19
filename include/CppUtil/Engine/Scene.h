#ifndef _ENGINE_SCENE_SCENE_H_
#define _ENGINE_SCENE_SCENE_H_

#include <CppUtil/Basic/Element.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class Camera;
		class Light;

		class Scene : public Basic::Element {
			ELE_SETUP(Scene)
		public:
			Scene(Basic::Ptr<SObj> root, const std::string & name = "")
				: root(root), name(name) { }

			Basic::Ptr<SObj> GetRoot() const { return root; }

			Basic::Ptr<Camera> GetCamera() const;

			std::vector<Basic::Ptr<Light>> GetLights() const;

			// sobjs have no same name
			// ID >= 1
			bool GenID();

			// ID >= 1, return 0 if error
			int GetID(Basic::Ptr<SObj> sobj) const;
			// ID >= 1, return "" if error
			std::string GetName(int ID) const;

		public:
			std::string name;

		private:
			Basic::Ptr<SObj> root;
			std::map<std::string, int> name2ID;
			std::map<int, std::string> ID2name;
		};
	}
}

#endif//!_ENGINE_SCENE_SCENE_H_
