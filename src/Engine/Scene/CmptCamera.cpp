#include <Engine/Scene/CmptCamera.h>

#include <Engine/Viewer/Ray.h>
#include <Engine/Scene/SObj.h>

#include <Basic/Math.h>

using namespace Ubpa;


using namespace Ubpa::Math;

void CmptCamera::Coordinate::Init(const transformf & tsfm) {
	pos = tsfm.decompose_position();
	front = (tsfm * normalf(0, 0, -1)).normalize();
	right = (tsfm * normalf(1, 0, 0)).normalize();
	up = (tsfm * normalf(0, 1, 0)).normalize();
}

CmptCamera::CmptCamera(Ptr<SObj> sobj, float fov, float ar, float near, float far)
	:
	Component(sobj),
	ar(ar),
	fov(fov),
	nearPlane(near),
	farPlane(far)
{
	h = 2 * tanf(fov / 2 / 180  * PI<float>);
	w = h * ar;
}

void CmptCamera::SetFOV(float fov) {
	this->fov = fov;
	h = 2 * tanf(fov / 2 / 180 * PI<float>);
	w = h * ar;
}

void CmptCamera::SetAspectRatio(float ar) {
	this->ar = ar;
	w = h * ar;
}

bool CmptCamera::InitCoordinate() {
	if (!GetSObj()) {
		coordinate.Init(transformf::eye());
		return false;
	}

	coordinate.Init(GetSObj()->GetLocalToWorldMatrix());
	return true;
}

const Ray CmptCamera::GenRay(float u, float v) const {
	const auto R = w * (u - 0.5f) * coordinate.right;
	const auto U = h * (v - 0.5f) * coordinate.up;
	return Ray(coordinate.pos, (coordinate.front + R + U).cast_to<vecf3>());
}
