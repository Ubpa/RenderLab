#include <CppUtil/Engine/Camera.h>

#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;
using namespace glm;

Camera::Camera(Basic::Ptr<SObj> sobj, float ar, float fov, float near, float far)
	: Component(sobj), ar(ar), fov(fov), nearClipping(near), farClipping(far) {
	h = 2 * tanf(fov / 2 / 180  * PI);
	w = h * ar;
}

void Camera::SetWidthHeight(int w, int h) {
	ar = float(w) / float(h);
	this->w = this->h * ar;
}

Ray::Ptr Camera::GenRay(float u, float v) {
	float x = w * (u - 0.5f);
	float y = h * (v - 0.5f);
	float z = -1;

	Ray::Ptr ray = ToPtr(new Ray(vec3(0), vec3(x, y, z)));
	return ray;
}
