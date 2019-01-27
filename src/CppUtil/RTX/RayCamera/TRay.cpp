#include <CppUtil/RTX/TRay.h>

using namespace RTX;
using namespace glm;

TRay::TRay(const vec3 & origin, const vec3 & dir, float time)
	: Ray(origin, dir), time(time) { }

float TRay::GetTime() const {
	return time;
}

void TRay::SetTime(float time) {
	this->time = time;
}

Ray::Ptr TRay::GenCopy() const {
	auto tRay = ToPtr(new TRay);
	CopyTo(tRay);
	return tRay;
}

void TRay::CopyTo(TRay::Ptr tRay) const {
	Ray::CopyTo(tRay);
	tRay->time = time;
}