#include <CppUtil/Engine/CmptCamera.h>

#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil::Basic::Math;

void CmptCamera::Coordinate::Init(const Transform & tsfm) {
	pos = tsfm.Position();
	front = tsfm(Normalf(0, 0, -1)).Norm();
	right = tsfm(Normalf(1, 0, 0)).Norm();
	up = tsfm(Normalf(0, 1, 0)).Norm();
}

CmptCamera::CmptCamera(Basic::Ptr<SObj> sobj, float fov, float ar, float near, float far)
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

void CmptCamera::SetFOV(float fov) {
	this->fov = fov;
	h = 2 * tanf(fov / 2 / 180 * PI);
	w = h * ar;
}

void CmptCamera::SetAspectRatio(float ar) {
	this->ar = ar;
	w = h * ar;
}

bool CmptCamera::InitCoordinate() {
	if (!GetSObj()) {
		coordinate.Init(Transform(1));
		return false;
	}

	coordinate.Init(GetSObj()->GetLocalToWorldMatrix());
	return true;
}

void CmptCamera::SetRay(ERay & ray, float u, float v) {
	const auto R = w * (u - 0.5f) * coordinate.right;
	const auto U = h * (v - 0.5f) * coordinate.up;
	ray.Init(coordinate.pos, coordinate.front + R + U);
}
