#include "GenScene.h"

#include <CppUtil/Engine/Engine.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Cube.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

Scene::Ptr GenScene() {
	auto sobj_Root = ToPtr(new SObj(nullptr));
	
	// camera
	auto sobj_Camera = ToPtr(new SObj(sobj_Root, "camera"));
	auto camera = ToPtr(new Camera(sobj_Camera));
	auto cameraTransform = ToPtr(new Transform(sobj_Camera));
	cameraTransform->SetPosition(vec3(1.3f, 1.f, 2.f));
	cameraTransform->Rotate(radians(37.0f), vec3(0, 1, 0));
	cameraTransform->Rotate(radians(-20.0f), vec3(1, 0, 0));

	// ground
	auto sobj_Ground = ToPtr(new SObj(sobj_Root, "ground"));
	auto plane = ToPtr(new Plane);
	auto planeG = ToPtr(new Geometry(sobj_Ground, plane));
	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));
	auto material0 = ToPtr(new Material(sobj_Ground, pink));
	auto planeTransform = ToPtr(new Transform(sobj_Ground));
	planeTransform->SetPosition(vec3(0, -0.5f, 0));
	planeTransform->SetScale(vec3(100.0f, 1.0f, 100.0f));

	// cube mesh
	auto sobj_Cube = ToPtr(new SObj(sobj_Root, "cube"));
	Cube cube;
	auto cubeMesh = ToPtr(new TriMesh(cube.GetTriNum(), cube.GetVertexNum(), 
		cube.GetIndexArr(), cube.GetPosArr(), cube.GetNormalArr()));
	auto cubeG = ToPtr(new Geometry(sobj_Cube, cubeMesh));
	auto blue = ToPtr(new BSDF_Diffuse(vec3(0.f, 0.794f, 0.916f)));
	auto material1 = ToPtr(new Material(sobj_Cube, blue));

	auto scene = ToPtr(new Scene(sobj_Root));
	return scene;
}
