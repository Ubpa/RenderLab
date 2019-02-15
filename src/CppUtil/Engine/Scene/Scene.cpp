#include <CppUtil/Engine/Scene.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/Light.h>

using namespace CppUtil::Engine;
using namespace std;

namespace CppUtil {
	namespace Engine {

	}
}

Scene::Scene(SObj::Ptr root, const string & name)
	: root(root), name(name), mainCamera(nullptr) { }

void Scene::Init() {
	mainCamera = root->GetComponentInChildren<Camera>();
	lights = root->GetComponentsInChildren<Light>();
}
