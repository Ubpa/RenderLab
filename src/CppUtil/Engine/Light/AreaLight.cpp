#include <CppUtil/Engine/AreaLight.h>

using namespace CppUtil::Engine;
using namespace glm;

vec3 AreaLight::Sample_L(const vec3 & p, vec3 & wi, float & distToLight, float & PD) const {
	if (p.y >= 0) {
		PD = 0;
		return vec3(0);
	}

	vec2 sample = vec2(sampler.GetSample() - vec2(0.5f, 0.5f));
	vec3 posOnLight(sample.x*width / 2, 0, sample.y*height / 2);

	vec3 d = posOnLight - p;
	float sqDist = dot(d, d);
	float dist = sqrt(sqDist);
	float cosTheta = d.y / dist;

	wi = d / dist;
	distToLight = dist;
	float area = width * height;
	PD = sqDist / (area * cosTheta);
	return intensity * color;
}

float AreaLight::PDF(const vec3 & p, const vec3 & wi) const {
	vec3 posOnLight;
	if (!Hit(p, wi, posOnLight))
		return 0;
	
	vec3 d = posOnLight - p;
	float sqDist = dot(d, d);
	float dist = sqrt(sqDist);
	float cosTheta = d.y / dist;
	float area = width * height;
	return sqDist / (area * cosTheta);
}

vec3 AreaLight::GetL(const vec3& p, const vec3 & dirToLight) {
	if (!Hit(p, dirToLight))
		return vec3(0);

	return intensity * color;
}

bool AreaLight::Hit(const glm::vec3& p, const glm::vec3 & dirToLight) const {
	if (p.y >= 0 || dirToLight.y < 0)
		return false;

	float yStep = p.y / dirToLight.y;
	float x = p.x - dirToLight.x * yStep;
	if (x >= width / 2 || x <= -width / 2)
		return false;

	float z = p.z - dirToLight.z * yStep;
	if (z >= height / 2 || z <= -height / 2)
		return false;

	return true;
}

bool AreaLight::Hit(const vec3& p, const vec3 & dirToLight, vec3 & hitPos) const {
	if (p.y >= 0 || dirToLight.y < 0)
		return false;

	float yStep = p.y / dirToLight.y;
	float x = p.x - dirToLight.x * yStep;
	if (x >= width / 2 || x <= -width / 2)
		return false;

	float z = p.z - dirToLight.z * yStep;
	if (z >= height / 2 || z <= -height / 2)
		return false;

	hitPos = vec3(x, 0, z);

	return true;
}
