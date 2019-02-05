#include "GenScene.h"

#include <CppUtil/Engine/Engine.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

Scene::Ptr GenScene() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj0 = ToPtr(new SObj(sobjRoot, "sobj0"));
	auto sobj1 = ToPtr(new SObj(sobjRoot, "sobj1"));
	
	auto camera = ToPtr(new Camera(sobjRoot));
	
	auto sphere0 = ToPtr(new Sphere(vec3(0, 0, -2), 0.5f));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -100, -2), 99.5f));

	auto g0 = ToPtr(new Geometry(sobj0, sphere0));
	auto g1 = ToPtr(new Geometry(sobj1, sphere1));

	auto pink = ToPtr(new BSDF_Mirror(vec3(1.0f,0.686f,0.788f)));
	auto gray = ToPtr(new BSDF_Diffuse(vec3(0.8f)));

	auto material0 = ToPtr(new Material(sobj0, pink));
	auto material1 = ToPtr(new Material(sobj1, gray));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene;
}