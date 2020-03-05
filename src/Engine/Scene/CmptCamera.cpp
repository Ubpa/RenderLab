#include <Engine/CmptCamera.h>

#include <Engine/Ray.h>
#include <Engine/SObj.h>

#include <Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil::Basic::Math;

void CmptCamera::Coordinate::Init(const Ubpa::transformf & tsfm) {
	pos = tsfm.decompose_position();
	front = (tsfm * Ubpa::normalf(0, 0, -1)).normalize();
	right = (tsfm * Ubpa::normalf(1, 0, 0)).normalize();
	up = (tsfm * Ubpa::normalf(0, 1, 0)).normalize();
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
		coordinate.Init(Ubpa::transformf::eye());
		return false;
	}

	coordinate.Init(GetSObj()->GetLocalToWorldMatrix());
	return true;
}

const ERay CmptCamera::GenRay(float u, float v) const {
	const auto R = w * (u - 0.5f) * coordinate.right;
	const auto U = h * (v - 0.5f) * coordinate.up;
	return ERay(coordinate.pos, (coordinate.front + R + U).cast_to<Ubpa::vecf3>());
}
