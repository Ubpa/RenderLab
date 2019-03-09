#include <CppUtil/Engine/Scene.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/Light.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

Camera::Ptr Scene::GetCamera() const {
	if (!root)
		return nullptr;

	return root->GetComponentInChildren<Camera>();
}

vector<Light::Ptr> Scene::GetLights() const {
	if (!root)
		return vector<Light::Ptr>();

	return root->GetComponentsInChildren<Light>();
}
