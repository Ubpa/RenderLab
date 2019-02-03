#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

// component
#include <CppUtil/Engine/Light.h>
#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/Transform.h>
#include <CppUtil/Engine/Geometry.h>
#include <CppUtil/Engine/Material.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/AreaLight.h>
#include <CppUtil/Engine/BSDF_Diffuse.h>

#include <iostream>
#include <string>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

int main() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj0 = ToPtr(new SObj(sobjRoot, "sobj0"));
	auto sobj1 = ToPtr(new SObj(sobj0, "sobj1"));
	auto sobj2 = ToPtr(new SObj(sobj0, "sobj2"));
	auto sobj3 = ToPtr(new SObj(sobj0, "sobj3"));

	cout << "sobjRoot has " << sobjRoot->GetChildren().size() << " children." << endl;
	cout << "sobj0 has " << sobj0->GetChildren().size() << " children." << endl;

	auto sphere = ToPtr(new Sphere(vec3(0, 0, -1), 1));
	auto geometry = ToPtr(new Geometry(sobj1, sphere));
	auto diffuse = ToPtr(new BSDF_Diffuse);
	auto material = ToPtr(new Material(sobj1, diffuse));
	cout << "sobj1 has " << sobj1->GetAllComponents().size() << " component." << endl;
	
	auto camera = ToPtr(new Camera(sobj2));

	auto arealight = ToPtr(new AreaLight);
	auto light = ToPtr(new Light(sobj3, arealight));

	auto scene = ToPtr(new Scene(sobjRoot, "test"));
	cout << "scene has " << (scene->GetMainCamera() == nullptr ?"not ":"") << "a camera." << endl;
	cout << "scene has " << scene->GetLights().size() << " lights." << endl;

	return 0;
}
