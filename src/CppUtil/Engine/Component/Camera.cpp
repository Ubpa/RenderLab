#include <CppUtil/Engine/Camera.h>

#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;
using namespace glm;

Camera::Camera(Basic::Ptr<SObj> sobj, float fov, float ar, float near, float far)
	: Component(sobj), ar(ar), fov(fov), nearPlane(near), farPlane(far) {
	h = 2 * tanf(fov / 2 / 180  * PI);
	w = h * ar;
}

void Camera::SetFOV(float fov) {
	this->fov = fov;
	h = 2 * tanf(fov / 2 / 180 * PI);
	w = h * ar;
}

void Camera::SetAspectRatio(float ar) {
	this->ar = ar;
	w = h * ar;
}

Ray::Ptr Camera::GenRay(float u, float v) {
	float x = w * (u - 0.5f);
	float y = h * (v - 0.5f);
	float z = -1;

	Ray::Ptr ray = ToPtr(new Ray(vec3(0), vec3(x, y, z)));
	return ray;
}
