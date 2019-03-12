#include <CppUtil/Engine/Camera.h>

#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;
using namespace glm;

void Camera::Coordinate::Init(const mat4 & matrix) {
	pos = matrix[3] / matrix[3][3];
	mat3 rotate = transpose(inverse(mat3(matrix)));
	front = normalize(rotate * vec3(0, 0, -1));
	right = normalize(rotate * vec3(1, 0, 0));
	up = normalize(rotate * vec3(0, 1, 0));
}

Camera::Camera(Basic::Ptr<SObj> sobj, float fov, float ar, float near, float far)
	:
	Component(sobj),
	ar(ar),
	fov(fov),
	nearPlane(near),
	farPlane(far)
{
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

bool Camera::InitCoordinate() {
	if (!GetSObj()) {
		coordinate.Init(mat4(1.f));
		return false;
	}

	coordinate.Init(GetSObj()->GetLocalToWorldMatrix());
	return true;
}

void Camera::SetRay(Ray::Ptr ray, float u, float v) {
	vec3 R = w * (u - 0.5f) * coordinate.right;
	vec3 U = h * (v - 0.5f) * coordinate.up;
	ray->Init(coordinate.pos, coordinate.front + R + U);
}
