#include <CppUtil/RTX/SceneCreator.h>

#include <CppUtil/OpenGL/CommonDefine.h>

#include <CppUtil/RTX/Scene.h>
#include <CppUtil/RTX/RayCamera.h>
#include <CppUtil/RTX/Ray.h>

#include <CppUtil/RTX/Sky.h>
#include <CppUtil/RTX/BVH_Node.h>
#include <CppUtil/RTX/Sphere.h>
#include <CppUtil/RTX/Group.h>

#include <CppUtil/RTX/OpMaterial.h>
#include <CppUtil/RTX/Lambertian.h>
#include <CppUtil/RTX/Metal.h>
#include <CppUtil/RTX/Dielectric.h>
#include <CppUtil/RTX/OpTexture.h>
#include <CppUtil/RTX/ImgTexture.h>

#include <CppUtil/Basic/Math.h>

#include <ROOT_PATH.h>

using namespace RTX;
using namespace Define;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

const string rootPath = ROOT_PATH;

Scene::CPtr SceneCreator::Gen(ENUM_SCENE scene, int w, int h) {
	switch (scene)
	{
	case RTX::SceneCreator::LOTS_OF_BALLS:
		return Gen_LOS_OF_BALLS(w, h);
		break;
	default:
		return nullptr;
		break;
	}
}

Scene::CPtr SceneCreator::Gen_LOS_OF_BALLS(int w, int h) {
	
	auto skyMat = ToPtr(new OpMaterial([](const HitRecord & rec)->bool {
		float t = 0.5f * (rec.vertex.pos.y + 1.0f);
		vec3 white = vec3(1.0f, 1.0f, 1.0f);
		vec3 blue = vec3(0.5f, 0.7f, 1.0f);
		vec3 lightColor = (1 - t) * white + t * blue;
		rec.ray->SetLightColor(lightColor);
		return false;
	}));
	auto sky = ToPtr(new Sky(skyMat));

	float t0 = 0.0f;
	float t1 = 1.0f;

	
	vector<Hitable::CPtr> bvhData;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = Math::Rand_F();
			vec3 center(a + 0.9*Math::Rand_F(), 0.2, b + 0.9*Math::Rand_F());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					auto mat = ToPtr(new Lambertian(vec3(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F())));
					auto sphere = ToPtr(new Sphere(center, 0.2f, mat));
					bvhData.push_back(sphere);
				}
				else if (choose_mat < 0.95) { // metal
					auto mat = ToPtr(new Metal(vec3(0.5f*(1.0f + Math::Rand_F()), 0.5f*(1.0f + Math::Rand_F()), 0.5f*(1.0f + Math::Rand_F())), 0.5f*Math::Rand_F()));
					auto sphere = ToPtr(new Sphere(center, 0.2f, mat));
					bvhData.push_back(sphere);
				}
				else {  // glass
					auto mat = ToPtr(new Dielectric(1.5));
					auto sphere = ToPtr(new Sphere(center, 0.2f, mat));
					bvhData.push_back(sphere);
				}
			}
		}
	}

	
	auto bvhNode = ToPtr(new BVH_Node(bvhData));

	
	auto checkTex = OpTexture::CheckerTexture(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));
	
	auto noiseTex = OpTexture::NoiseTexture(0, vec3(1), 3);
	
	auto earthTex = ToPtr(new ImgTexture(rootPath + texture_earth, true));

	
	if (!earthTex->IsValid()) {
		printf("ERROR: earthTex[%s] is invalid.\n", (rootPath + texture_earth).c_str());
		exit(1);
	}
	

	auto group = ToPtr(new Group);
	auto sphereBottom = ToPtr(new Sphere(vec3(0, -1000, 0), 1000, ToPtr(new Lambertian(noiseTex))));
	auto sphere0 = ToPtr(new Sphere(vec3(6, 1, 0), 1.0, ToPtr(new Metal(vec3(0.7, 0.6, 0.5), 0.0))));
	auto sphere1 = ToPtr(new Sphere(vec3(2, 1, 0), 1.0, ToPtr(new Dielectric(1.5f))));
	auto sphere2 = ToPtr(new Sphere(vec3(2, 1, 0), -0.8f, ToPtr(new Dielectric(1.5f))));
	auto sphere3 = ToPtr(new Sphere(vec3(-2, 1, 0), 1.0, ToPtr(new Lambertian(earthTex))));
	auto sphere4 = ToPtr(new Sphere(vec3(-6, 1, 0), 1.0, ToPtr(new Lambertian(checkTex))));

	(*group) << bvhNode << sphere0 << sphere1 << sphere2 << sphere3 << sphere4 << sphereBottom << sky;

	
	vec3 origin(13, 2, 3);
	vec3 viewPoint(0, 0, 0);
	float fov = 20.0f;
	float lenR = 0.05f;
	float distToFocus = 10.0f;
	float ratioWH = float(w) / float(h);
	RayCamera::Ptr camera = ToPtr(new RayCamera(origin, viewPoint, ratioWH, fov, lenR, distToFocus));

	return ToPtr(new Scene(group, camera));
}