#include <CppUtil/RTX/AABB.h>

#include <CppUtil/RTX/Ray.h>

using namespace RTX;
using namespace glm;

const AABB AABB::InValid(vec3(1), vec3(-1), false);

AABB::AABB(){
	*this = InValid;
}

AABB::AABB(const vec3 & minP, const vec3 & maxP, bool isValid)
	: minP(minP), maxP(maxP), isValid(isValid) { }

float AABB::GetSurfaceArea() const {
	vec3 extent = maxP - minP;
	return 2 * (extent.x * extent.y + extent.x*extent.z + extent.y*extent.z);
}

void AABB::SetP(const glm::vec3 & minP, const glm::vec3 & maxP) {
	this->minP = minP;
	this->maxP = maxP;
	isValid = true;
}

bool AABB::Hit(Ray::Ptr ray, float & tMin, float &tMax) const {
	if (!IsValid())
		return false;

	const vec3 origin = ray->GetOrigin();
	const vec3 dir = ray->GetDir();
	tMin = Ray::tMin;
	tMax = ray->GetTMax();
	for (size_t i = 0; i < 3; i++) {
		float invD = 1.0f / dir[i];
		float t0 = (minP[i] - origin[i]) * invD;
		float t1 = (maxP[i] - origin[i]) * invD;
		if (invD < 0.0f)
			std::swap(t0, t1);

		tMin = max(t0, tMin);
		tMax = min(t1, tMax);
		if (tMax <= tMin)
			return false;
	}
	return true;
}

bool AABB::Hit(Ray::Ptr ray) const {
	float tMin, tMax;
	return Hit(ray, tMin, tMax);
}

const AABB AABB::operator+(const AABB & aabb) const{
	if (isValid) {
		if (aabb.isValid) {
			vec3 minP = min(this->minP, aabb.minP);
			vec3 maxP = max(this->maxP, aabb.maxP);

			return { minP, maxP };
		}
		else
			return *this;
	}
	else {
		if (aabb.isValid)
			return aabb;
		else
			return AABB::InValid;
	}
}

void AABB::Expand(const AABB & aabb) {
	if (aabb.isValid) {
		if (isValid) {
			minP = min(minP, aabb.minP);
			maxP = max(maxP, aabb.maxP);
		}
		else {
			minP = aabb.minP;
			maxP = aabb.maxP;
			isValid = true;
		}
	}
}

AABB & AABB::operator +=(const AABB & aabb) {
	Expand(aabb);

	return *this;
}