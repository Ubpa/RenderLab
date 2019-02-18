#include <CppUtil/RTX/TRayCamera.h>

#include <CppUtil/RTX/TRay.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Basic;
using namespace RTX;
using namespace glm;

TRayCamera::TRayCamera(const glm::vec3 & pos, const glm::vec3 & viewPoint, float ratioWH,
	float t0, float t1,
	float fov, float lenR, float focus_dist,
	const glm::vec3 & worldUp)
	: RayCamera(pos, viewPoint, ratioWH, fov, lenR, focus_dist, worldUp),
	t0(t0), t1(t1){
}

Ray::Ptr TRayCamera::GenRay(float s, float t) const{
	auto tRay = ToPtr(new TRay);
	RayCamera::GenRay(s, t, tRay);

	float time = t0 + Math::Rand_F()*(t1 - t0);
	tRay->SetTime(time);

	return tRay;
}