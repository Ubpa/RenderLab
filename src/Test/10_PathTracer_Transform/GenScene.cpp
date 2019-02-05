#include "GenScene.h"

#include <CppUtil/Engine/Engine.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

Scene::Ptr GenScene() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj_camera = ToPtr(new SObj(sobjRoot, "camera"));
	auto sobj_sphere = ToPtr(new SObj(sobjRoot, "sobj_sphere"));
	auto sobj_ground = ToPtr(new SObj(sobjRoot, "sobj_ground"));
	
	auto cameraTransform = ToPtr(new Transform(sobj_camera));
	cameraTransform->SetPosition(vec3(0, 0, 3));
	auto camera = ToPtr(new Camera(sobj_camera));
	
	auto sphere0 = ToPtr(new Sphere(vec3(0, 0, 0), 0.5f));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -100, 0), 99.5f));

	auto sphereTransform = ToPtr(new Transform(sobj_sphere));
	sphereTransform->SetScale(vec3(2, 1, 1));
	sphereTransform->Rotate(radians(45.0f), vec3(0, 1, 0));

	auto g0 = ToPtr(new Geometry(sobj_sphere, sphere0));
	auto g1 = ToPtr(new Geometry(sobj_ground, sphere1));

	auto glass = ToPtr(new BSDF_Glass(1.2f));
	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new Material(sobj_sphere, glass));
	auto material1 = ToPtr(new Material(sobj_ground, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene;
}
