#include "GenScene.h"

#include <CppUtil/Engine/Engine.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Cube.h>

#include <ROOT_PATH.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

Ptr<SObj> GenBox() {
	auto sobj_Box = SObj::New(nullptr, "Cornell Box");

	// wall
	const int wallNum = 5;
	Ptr<SObj> sobj_walls[wallNum] = {
		SObj::New(sobj_Box, "wall ceil"),
		SObj::New(sobj_Box, "wall down"),
		SObj::New(sobj_Box, "wall left"),
		SObj::New(sobj_Box, "wall right"),
		SObj::New(sobj_Box, "wall back")
	};

	Point3 posArr[wallNum] = {
		Point3(0, 1.5f, 0),
		Point3(0, 0, 0),
		Point3(-1, 0.75f, 0),
		Point3(1, 0.75f, 0),
		Point3(0, 0.75f, -1),
	};

	Vec3 axisArr[wallNum] = {
		Vec3(0, 0, 1),
		Vec3(0, 1, 0),
		Vec3(0, 0, 1),
		Vec3(0, 0, 1),
		Vec3(1, 0, 0),
	};

	float degreeArr[wallNum] = {
		180,
		0,
		-90,
		90,
		90,
	};

	Vec3 scaleArr[wallNum] = {
		Vec3(2, 1, 2),
		Vec3(2, 1, 2),
		Vec3(1.5f, 1, 2),
		Vec3(1.5f, 1, 2),
		Vec3(2, 1, 1.5f),
	};

	RGBf colorArr[wallNum] = {
		RGBf(0.6f),
		RGBf(0.6f),
		RGBf(0.6f, 0.2f, 0.2f),
		RGBf(0.2f, 0.2f, 0.6f),
		RGBf(0.6f),
	};

	for (int i = 0; i < wallNum; i++) {
		auto bsdfDiffuse = BSDF_Diffuse::New(colorArr[i]);
		auto materialDiffuse = CmptMaterial::New(sobj_walls[i], bsdfDiffuse);

		auto plane = Plane::New();
		auto geo = CmptGeometry::New(sobj_walls[i], plane);

		auto transform = CmptTransform::New(sobj_walls[i]);
		transform->SetPosition(posArr[i]);
		transform->SetRotation(axisArr[i], degreeArr[i]);
		transform->SetScale(scaleArr[i]);
	}

	// light
	auto sobj_AreaLight = SObj::New(sobj_Box, "area light");
	auto areaLight = AreaLight::New(RGBf(1), 15, 0.8f, 0.6f);
	auto light = CmptLight::New(sobj_AreaLight, areaLight);
	auto lightTransform = CmptTransform::New(sobj_AreaLight);
	lightTransform->SetPosition(Point3(0, 1.49f, 0));
	lightTransform->SetScale(Vec3(0.8f, 1.0f, 0.6f));
	auto lightPlane = Plane::New();
	auto lightGeo = CmptGeometry::New(sobj_AreaLight, lightPlane);
	auto bsdfEmission = BSDF_Emission::New(RGBf(2));
	auto materailEmission = CmptMaterial::New(sobj_AreaLight, bsdfEmission);

	return sobj_Box;
}

Ptr<SObj> GenGound() {
	auto sobj_ground = SObj::New(nullptr, "ground");
	auto material = CmptMaterial::New(sobj_ground, BSDF_Diffuse::New());
	auto plane = CmptGeometry::New(sobj_ground, Plane::New());
	auto transform = CmptTransform::New(sobj_ground);
	transform->SetScale(Vec3(100, 1, 100));
	return sobj_ground;
}

Ptr<Scene> GenScene00() {
	auto sobj_Root = SObj::New(nullptr, "root");
	auto sobj_Camera = SObj::New(sobj_Root, "camera");
	auto sobj_MirrorSphere = SObj::New(sobj_Root, "mirror sphere");
	auto sobj_GlassSphere = SObj::New(sobj_Root, "glass sphere");
	auto sobj_AreaLight = SObj::New(sobj_Root, "area light");
	const int wallNum = 5;
	Ptr<SObj> sobj_walls[wallNum] = {
		SObj::New(sobj_Root, "wall ceil"),
		SObj::New(sobj_Root, "wall down"),
		SObj::New(sobj_Root, "wall left"),
		SObj::New(sobj_Root, "wall right"),
		SObj::New(sobj_Root, "wall back")
	};

	// camera
	auto camera = CmptCamera::New(sobj_Camera, 50.0f);
	auto cameraTransform = CmptTransform::New(sobj_Camera);
	cameraTransform->SetPosition(Point3(0, 0.75f, 2.4f));

	// light
	auto areaLight = AreaLight::New(RGBf(1), 15, 0.8f, 0.6f);
	auto light = CmptLight::New(sobj_AreaLight, areaLight);
	auto lightTransform = CmptTransform::New(sobj_AreaLight);
	lightTransform->SetPosition(Point3(0, 1.49f, 0));
	lightTransform->SetScale(Vec3(0.8f, 1.0f, 0.6f));
	auto lightPlane = Plane::New();
	auto lightGeo = CmptGeometry::New(sobj_AreaLight, lightPlane);
	auto bsdfEmission = BSDF_Emission::New(RGBf(2));
	auto materailEmission = CmptMaterial::New(sobj_AreaLight, bsdfEmission);

	// mirror sphere
	auto bsdfMirror = BSDF_Mirror::New();
	auto materialMirror = CmptMaterial::New(sobj_MirrorSphere, bsdfMirror);

	auto mirrorSphereTransform = CmptTransform::New(sobj_MirrorSphere);
	mirrorSphereTransform->SetPosition(Point3(-0.4f, 0.3f, -0.3f));
	mirrorSphereTransform->SetScale(Vec3(0.3f));

	auto geoMetalSphere = CmptGeometry::New(sobj_MirrorSphere, Sphere::New());

	// glass sphere
	auto bsdfGlass = BSDF_Glass::New(1.45f);
	auto materialGlass = CmptMaterial::New(sobj_GlassSphere, bsdfGlass);

	auto glassSphereTransform = CmptTransform::New(sobj_GlassSphere);
	glassSphereTransform->SetPosition(Point3(0.4f, 0.3f, 0.3f));
	glassSphereTransform->SetScale(Vec3(0.3f));

	auto geoGlassSphere = CmptGeometry::New(sobj_GlassSphere, Sphere::New());

	// mesh . cube 
	auto sobj_Cube = SObj::New(sobj_Root, "cube");
	Cube cube;
	auto cubeMesh = TriMesh::New(cube.GetTriNum(), cube.GetVertexNum(),
		cube.GetIndexArr(), cube.GetPosArr(), cube.GetNormalArr(), cube.GetTexCoordsArr());
	auto cubeG = CmptGeometry::New(sobj_Cube, cubeMesh);
	auto blueGlass = BSDF_Glass::New(1.5f, RGBf(0.f, 0.794f, 0.916f));
	auto materialCube = CmptMaterial::New(sobj_Cube, blueGlass);
	auto cubeTransform = CmptTransform::New(sobj_Cube);
	cubeTransform->SetScale(Vec3(0.2f));
	cubeTransform->SetPosition(Point3(0, 0.15f, 0));

	sobj_Root->AddChild(GenBox());

	auto scene = Scene::New(sobj_Root, "scene 00");
	return scene;
}

Ptr<Scene> GenScene01() {
	auto sobjRoot = SObj::New(nullptr, "root");
	auto sobj0 = SObj::New(sobjRoot, "sobj0");
	auto sobj1 = SObj::New(sobjRoot, "sobj1");

	auto camera = CmptCamera::New(sobjRoot);

	auto sphere0Transform = CmptTransform::New(sobj0, Point3(0,0,-2), Vec3(0.5f));
	auto sphere0 = Sphere::New();
	auto sphere1Transform = CmptTransform::New(sobj0, Point3(0, -100, -2), Vec3(99.5f));
	auto sphere1 = Sphere::New();

	auto g0 = CmptGeometry::New(sobj0, sphere0);
	auto g1 = CmptGeometry::New(sobj1, sphere1);

	auto pink = BSDF_Diffuse::New(RGBf(1.0f, 0.686f, 0.788f));
	auto gray = BSDF_Diffuse::New(RGBf(0.8f));

	auto material0 = CmptMaterial::New(sobj0, pink);
	auto material1 = CmptMaterial::New(sobj1, gray);

	auto scene = Scene::New(sobjRoot, "scene 01");
	return scene;
}

Ptr<Scene> GenScene02() {
	auto sobjRoot = SObj::New(nullptr, "root");
	auto sobj0 = SObj::New(sobjRoot, "sobj0");
	auto sobj1 = SObj::New(sobjRoot, "sobj1");

	auto camera = CmptCamera::New(sobjRoot);

	auto sphere0Transform = CmptTransform::New(sobj0, Point3(0, 0, -2), Vec3(0.5f));
	auto sphere0 = Sphere::New();
	auto sphere1Transform = CmptTransform::New(sobj0, Point3(0, -100, -2), Vec3(99.5f));
	auto sphere1 = Sphere::New();

	auto g0 = CmptGeometry::New(sobj0, sphere0);
	auto g1 = CmptGeometry::New(sobj1, sphere1);

	auto white = BSDF_Mirror::New(RGBf(1.0f));
	auto pink = BSDF_Diffuse::New(RGBf(1.0f, 0.686f, 0.788f));

	auto material0 = CmptMaterial::New(sobj0, white);
	auto material1 = CmptMaterial::New(sobj1, pink);

	auto scene = Scene::New(sobjRoot, "scene 02");
	return scene;
}

Ptr<Scene> GenScene03(){
	auto sobjRoot = SObj::New(nullptr, "root");
	auto sobj0 = SObj::New(sobjRoot, "sobj0");
	auto sobj1 = SObj::New(sobjRoot, "sobj1");

	auto camera = CmptCamera::New(sobjRoot);

	auto sphere0Transform = CmptTransform::New(sobj0, Point3(0, 0, -2), Vec3(0.5f));
	auto sphere0 = Sphere::New();
	auto sphere1Transform = CmptTransform::New(sobj0, Point3(0, -100, -2), Vec3(99.5f));
	auto sphere1 = Sphere::New();

	auto g0 = CmptGeometry::New(sobj0, sphere0);
	auto g1 = CmptGeometry::New(sobj1, sphere1);

	auto glass = BSDF_Glass::New(1.5f);
	auto pink = BSDF_Diffuse::New(RGBf(1.0f, 0.686f, 0.788f));

	auto material0 = CmptMaterial::New(sobj0, glass);
	auto material1 = CmptMaterial::New(sobj1, pink);

	auto scene = Scene::New(sobjRoot, "scene 03");
	return scene;
}

Ptr<Scene> GenScene04() {
	auto sobjRoot = SObj::New(nullptr, "root");
	auto sobj_camera = SObj::New(sobjRoot, "camera");
	auto sobj_sphere = SObj::New(sobjRoot, "sobj_sphere");
	auto sobj_ground = SObj::New(sobjRoot, "sobj_ground");

	auto cameraTransform = CmptTransform::New(sobj_camera);
	cameraTransform->SetPosition(Point3(0, 0, 3));
	auto camera = CmptCamera::New(sobj_camera);

	auto sphere0Transform = CmptTransform::New(sobj_sphere, Point3(0, 0, 0), Vec3(1, 0.5,0.5), Vec3(0,1,0), 45);
	auto sphere0 = Sphere::New();
	auto sphere1Transform = CmptTransform::New(sobj_ground, Point3(0, -100, 0), Vec3(99.5f));
	auto sphere1 = Sphere::New();

	auto g0 = CmptGeometry::New(sobj_sphere, sphere0);
	auto g1 = CmptGeometry::New(sobj_ground, sphere1);

	auto glass = BSDF_Glass::New(1.2f);
	auto pink = BSDF_Diffuse::New(RGBf(1.0f, 0.686f, 0.788f));

	auto material0 = CmptMaterial::New(sobj_sphere, glass);
	auto material1 = CmptMaterial::New(sobj_ground, pink);

	auto scene = Scene::New(sobjRoot, "scene 04");
	return scene;
}

Ptr<Scene> GenScene05() {
	auto sobjRoot = SObj::New(nullptr, "root");
	auto sobj_camera = SObj::New(sobjRoot, "camera");
	auto sobj_sphere = SObj::New(sobjRoot, "sobj_sphere");
	auto sobj_ground = SObj::New(sobjRoot, "sobj_ground");

	auto cameraTransform = CmptTransform::New(sobj_camera);
	cameraTransform->SetPosition(Point3(0, 0.1f, 3));
	auto camera = CmptCamera::New(sobj_camera);

	auto sphereTransform = CmptTransform::New(sobj_sphere, Point3(0, 0, -2), Vec3(0.5f), Vec3(0,1,0), 30);
	auto sphere = Sphere::New();
	auto plane = Plane::New();

	auto planeTransform = CmptTransform::New(sobj_ground);
	planeTransform->SetScale(Vec3(10.0f, 1.f, 10.0f));

	auto g0 = CmptGeometry::New(sobj_sphere, sphere);
	auto g1 = CmptGeometry::New(sobj_ground, plane);

	auto glass = BSDF_Glass::New(1.2f);
	auto pink = BSDF_Diffuse::New(RGBf(1.0f, 0.686f, 0.788f));

	auto material0 = CmptMaterial::New(sobj_sphere, glass);
	auto material1 = CmptMaterial::New(sobj_ground, pink);

	auto scene = Scene::New(sobjRoot, "scene05");
	return scene; 
}

Ptr<Scene> GenScene06() {
	auto sobjRoot = SObj::New(nullptr, "root");
	auto sobj_camera = SObj::New(sobjRoot, "camera");
	auto sobj_skySphere = SObj::New(sobjRoot, "sobj_skySphere");
	auto sobj_sphere = SObj::New(sobjRoot, "sobj_sphere");
	auto sobj_light = SObj::New(sobjRoot, "sobj_light");
	auto sobj_ground = SObj::New(sobjRoot, "sobj_ground");

	auto cameraTransform = CmptTransform::New(sobj_camera);
	cameraTransform->SetPosition(Point3(0, 0.5f, 3));
	auto camera = CmptCamera::New(sobj_camera);

	auto skySphereTransform = CmptTransform::New(sobj_sphere, Point3(0), Vec3(4));
	auto skySphere = Sphere::New();
	auto sphereTransform = CmptTransform::New(sobj_sphere, Point3(0), Vec3(0.5f));
	auto sphere = Sphere::New();
	auto plane = Plane::New();
	auto lightPlane = Plane::New();
	auto areaLight = AreaLight::New(RGBf(1), 10);

	auto planeTransform = CmptTransform::New(sobj_ground);
	planeTransform->SetScale(Vec3(100.0f, 1.f, 100.0f));
	planeTransform->SetPosition(Point3(0, -1, 0));

	auto lightTransform = CmptTransform::New(sobj_light);
	lightTransform->SetPosition(Point3(0, 2.f, 0));

	auto light = CmptLight::New(sobj_light, areaLight);

	auto g0 = CmptGeometry::New(sobj_skySphere, skySphere);
	auto g1 = CmptGeometry::New(sobj_ground, plane);
	auto g2 = CmptGeometry::New(sobj_sphere, sphere);
	auto g3 = CmptGeometry::New(sobj_light, lightPlane);

	auto sky = BSDF_Diffuse::New(RGBf(0));
	auto glass = BSDF_Glass::New(1.5f);
	auto pink = BSDF_Diffuse::New(RGBf(1.0f, 0.686f, 0.788f));
	auto emission = BSDF_Emission::New(RGBf(1));

	auto material0 = CmptMaterial::New(sobj_skySphere, sky);
	auto material1 = CmptMaterial::New(sobj_ground, pink);
	auto material2 = CmptMaterial::New(sobj_sphere, glass);
	auto material3 = CmptMaterial::New(sobj_light, emission);

	auto scene = Scene::New(sobjRoot, "scene 06");
	return scene;
}

Ptr<Scene> GenScene07() {
	auto sobjRoot = SObj::New(nullptr, "root");

	auto sobj_Camera = SObj::New(sobjRoot, "camera");
	auto camera = CmptCamera::New(sobj_Camera, 20.0f);
	auto cameraTransform = CmptTransform::New(sobj_Camera);
	cameraTransform->LookAt(Point3(13, 2, 3), Point3(0));

	auto balls = SObj::New(sobjRoot, "balls");
	for (int a = -11, id = 0; a < 11; a++) {
		for (int b = -11; b < 11; b++, id++) {
			auto ball = SObj::New(balls, "ball " + to_string(id));
			Point3 center(a + 0.9*Math::Rand_F(), 0.2, b + 0.9*Math::Rand_F());
			auto tsfm = CmptTransform::New(ball, center, Vec3(0.2f));

			auto geo = CmptGeometry::New(ball, Sphere::New());

			Ptr<BSDFBase> bsdf;
			float choose_mat = Math::Rand_F();
			if (choose_mat < 0.8) {  // diffuse
				RGBf color(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F());
				bsdf = BSDF_Diffuse::New(color);
			}
			else if (choose_mat < 0.95) { // metal
				RGBf color(0.5f*(1 + Math::Rand_F()), 0.5f*(1 + Math::Rand_F()), 0.5f*(1 + Math::Rand_F()));
				bsdf = BSDF_Mirror::New(color);
			}
			else {  // glass
				bsdf = BSDF_Glass::New(1.2f + Math::Rand_F()*0.5f);
			}

			auto material = CmptMaterial::New(ball, bsdf);
		}
	}


	Point3 center[6] = {
		Point3(0, -1000, 0),
		Point3(6, 1, 0),
		Point3(2, 1, 0),
		Point3(2, 1, 0),
		Point3(-2, 1, 0),
		Point3(-6, 1, 0),
	};
	float radius[6] = { 1000.f,1.f,1.f,-0.8f,1.f,1.f };
	Ptr<BSDFBase> bsdf[6] = {
		BSDF_Diffuse::New(RGBf(0.5f)),
		BSDF_Mirror::New(RGBf(0.7f, 0.6f, 0.5f)),
		BSDF_Glass::New(1.5f),
		BSDF_Glass::New(1.5f),
		BSDF_Diffuse::New(RGBf(0.4f,0.2f,0.1f)),
		BSDF_Glass::New(2.5f),
	};
	for (int i = 0; i < 6; i++) {
		auto sobj = SObj::New(sobjRoot, "sphere" + to_string(i));
		auto tsfm = CmptTransform::New(sobj, center[i], Vec3(radius[i]));
		auto geo = CmptGeometry::New(sobj, Sphere::New());
		auto material = CmptMaterial::New(sobj, bsdf[i]);
	}

	return Scene::New(sobjRoot, "scene 07");
}

Ptr<Scene> GenScene08() {
	auto sobj_Root = SObj::New(nullptr, "root");

	// camera
	auto sobj_Camera = SObj::New(sobj_Root, "camera");
	auto camera = CmptCamera::New(sobj_Camera, 50.0f);
	auto cameraTransform = CmptTransform::New(sobj_Camera);
	cameraTransform->SetPosition(Point3(0, 0.75f, 2.4f));

	// light
	auto sobj_AreaLight = SObj::New(sobj_Root, "area light");
	auto areaLight = AreaLight::New(RGBf(1), 15, 0.8f, 0.6f);
	auto light = CmptLight::New(sobj_AreaLight, areaLight);
	auto lightTransform = CmptTransform::New(sobj_AreaLight);
	lightTransform->SetPosition(Point3(0, 1.49f, 0));
	lightTransform->SetScale(Vec3(0.8f, 1.0f, 0.6f));
	auto lightPlane = Plane::New();
	auto lightGeo = CmptGeometry::New(sobj_AreaLight, lightPlane);
	auto bsdfEmission = BSDF_Emission::New(RGBf(2));
	auto materailEmission = CmptMaterial::New(sobj_AreaLight, bsdfEmission);

	// wall

	const int wallNum = 5;
	Ptr<SObj> sobj_walls[wallNum] = {
		SObj::New(sobj_Root, "wall ceil"),
		SObj::New(sobj_Root, "wall down"),
		SObj::New(sobj_Root, "wall left"),
		SObj::New(sobj_Root, "wall right"),
		SObj::New(sobj_Root, "wall back")
	};

	Point3 posArr[wallNum] = {
		Point3(0, 1.5f, 0),
		Point3(0, 0, 0),
		Point3(-1, 0.75f, 0),
		Point3(1, 0.75f, 0),
		Point3(0, 0.75f, -1),
	};

	Vec3 axisArr[wallNum] = {
		Vec3(0, 1, 0),
		Vec3(0, 1, 0),
		Vec3(0, 0, 1),
		Vec3(0, 0, 1),
		Vec3(1, 0, 0),
	};

	float degreeArr[wallNum] = {
		0,
		0,
		90,
		90,
		90,
	};

	Vec3 scaleArr[wallNum] = {
		Vec3(2, 1, 2),
		Vec3(2, 1, 2),
		Vec3(1.5f, 1, 2),
		Vec3(1.5f, 1, 2),
		Vec3(2, 1, 1.5f),
	};

	RGBf colorArr[wallNum] = {
		RGBf(0.6f),
		RGBf(0.6f),
		RGBf(0.6f, 0.2f, 0.2f),
		RGBf(0.2f, 0.2f, 0.6f),
		RGBf(0.6f),
	};

	for (int i = 0; i < wallNum; i++) {
		auto bsdfDiffuse = BSDF_Diffuse::New(colorArr[i]);
		auto materialDiffuse = CmptMaterial::New(sobj_walls[i], bsdfDiffuse);

		auto plane = Plane::New();
		auto geo = CmptGeometry::New(sobj_walls[i], plane);

		auto transform = CmptTransform::New(sobj_walls[i], posArr[i], scaleArr[i], axisArr[i], degreeArr[i]);
	}

	// cook torrance sphere
	auto sobj_CTSphere = SObj::New(sobj_Root, "cook torrance sphere");

	auto bsdfCookTorrance = BSDF_CookTorrance::New(10.f, 0.4f, RGBf(0.1f), RGBf(1.0f,0.3f,0.5f));
	auto materialCookTorrance = CmptMaterial::New(sobj_CTSphere, bsdfCookTorrance);

	auto CTSphereTransform = CmptTransform::New(sobj_CTSphere);
	CTSphereTransform->SetPosition(Point3(0, 0.3f, 0));
	CTSphereTransform->SetScale(Vec3(0.3f));

	auto geoCTSphere = CmptGeometry::New(sobj_CTSphere, Sphere::New());

	// sky sphere
	auto sobj_skySphere = SObj::New(sobj_Root, "sky");
	auto skyTsfm = CmptTransform::New(sobj_skySphere, Point3(0), Vec3(100));
	auto geoSky = CmptGeometry::New(sobj_skySphere, Sphere::New());
	auto dark = BSDF_Diffuse::New(RGBf(0));
	auto materialSky = CmptMaterial::New(sobj_skySphere, dark);

	return Scene::New(sobj_Root, "scene 08");
}

Ptr<Scene> GenScene09() {
	auto sobj_Root = SObj::New(nullptr, "root");

	// metal workflow sphere
	auto sobj_MWSphere = SObj::New(sobj_Root, "metal workflow sphere");

	RGBf gold(1.00, 0.71, 0.29);
	auto bsdfGold = BSDF_MetalWorkflow::New(gold, 0.2f);
	auto materialGold = CmptMaterial::New(sobj_MWSphere, bsdfGold);

	auto MWSphereTransform = CmptTransform::New(sobj_MWSphere);
	MWSphereTransform->SetPosition(Point3(0, 0.3f, 0));
	MWSphereTransform->SetScale(Vec3(0.3f));

	auto geoMWSphere = CmptGeometry::New(sobj_MWSphere, Sphere::New());

	// cornell box
	auto box = GenBox();
	auto boxTransform = CmptTransform::New(box);
	sobj_Root->AddChild(box);

	// ground
	//auto ground = GenGound();
	//ground->SetParent(sobj_Root);

	// plane

	// camera
	auto sobj_Camera = SObj::New(sobj_Root, "camera");
	auto camera = CmptCamera::New(sobj_Camera, 50.0f);
	auto cameraTransform = CmptTransform::New(sobj_Camera);
	cameraTransform->SetPosition(Point3(0, 0.75f, 2.4f));

	// sky sphere
	auto sobj_skySphere = SObj::New(sobj_Root, "sky");
	auto skyTsfm = CmptTransform::New(sobj_skySphere, Point3(0), Vec3(100));
	auto geoSky = CmptGeometry::New(sobj_skySphere, Sphere::New());
	auto dark = BSDF_Diffuse::New(RGBf(0));
	auto materialSky = CmptMaterial::New(sobj_skySphere, dark);

	return Scene::New(sobj_Root, "scene 09");
}

Ptr<Scene> GenScene10() {
	auto sobj_Root = SObj::New(nullptr, "root");

	// rusted iron workflow sphere
	auto sobj_IronSphere = SObj::New(sobj_Root, "rusted iron sphere");

	auto bsdfIron = BSDF_MetalWorkflow::New(RGBf(1));
	auto ironAlbedoImg = Image::New((ROOT_PATH+"data/textures/pbr/rusted_iron/albedo.png").c_str());
	bsdfIron->SetAlbedoTexture(ironAlbedoImg);
	auto ironMatallicImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/metallic.png").c_str());
	bsdfIron->SetMetallicTexture(ironMatallicImg);
	auto ironRoughnessImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/roughness.png").c_str());
	bsdfIron->SetRoughnessTexture(ironRoughnessImg);
	auto ironAOImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/ao.png").c_str());
	bsdfIron->SetAOTexture(ironAOImg);
	auto ironNormalImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/normal.png").c_str());
	bsdfIron->SetNormalTexture(ironNormalImg);
	auto materialIron = CmptMaterial::New(sobj_IronSphere, bsdfIron);

	auto MWSphereTransform = CmptTransform::New(sobj_IronSphere, Point3(0, 0.3f, 0), Vec3(0.3f));
	MWSphereTransform->Rotate(Vec3(0, 1, 0), 90.f);
	MWSphereTransform->Rotate(Vec3(1, 0, 0), 180.f);

	auto geoMWSphere = CmptGeometry::New(sobj_IronSphere, Sphere::New());

	// cornell box
	auto box = GenBox();
	auto boxTransform = CmptTransform::New(box);
	sobj_Root->AddChild(box);

	// ground
	//auto ground = GenGound();
	//ground->SetParent(sobj_Root);

	// plane

	// camera
	auto sobj_Camera = SObj::New(sobj_Root, "camera");
	auto camera = CmptCamera::New(sobj_Camera, 50.0f);
	auto cameraTransform = CmptTransform::New(sobj_Camera, Point3(0, 0.5f, 0.8f));
	cameraTransform->SetPosition(Point3(0, 0.5f, 0.8f));
	cameraTransform->Rotate(Vec3(1, 0, 0), -15.f);

	// sky sphere
	auto sobj_skySphere = SObj::New(sobj_Root, "sky");
	auto skyTsfm = CmptTransform::New(sobj_skySphere, Point3(0), Vec3(100));
	auto geoSky = CmptGeometry::New(sobj_skySphere, Sphere::New());
	auto dark = BSDF_Diffuse::New(RGBf(0));
	auto materialSky = CmptMaterial::New(sobj_skySphere, dark);

	return Scene::New(sobj_Root, "scene 10");
}

Ptr<Scene> GenScene(int n) {
	const int num = 11;
	Ptr<Scene>(*f[num])() = {
		&GenScene00,
		&GenScene01,
		&GenScene02,
		&GenScene03,
		&GenScene04,
		&GenScene05,
		&GenScene06,
		&GenScene07,
		&GenScene08,
		&GenScene09,
		&GenScene10,
	};

	if (n < num)
		return f[n]();
	else
		return nullptr;
}
