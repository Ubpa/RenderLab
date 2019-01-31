#include <CppUtil/Engine/Camera.h>

#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

Camera::Camera(Basic::Ptr<SObj> sobj, float ar, float fov, float near, float far)
	: Component(sobj), ar(ar), fov(fov), near(near), far(far) {
	w = 2 * tanf(fov / 180 / 2 * Math::PI);
	h = w / ar;
}

Ray::Ptr Camera::GenRay(float u, float v) {
	float x = w * (u - 0.5f);
	float y = h * (v - 0.5f);
	float z = -1;

	Ray::Ptr ray = ToPtr(new Ray(vec3(0), vec3(x, y, z)));
	return ray;
}