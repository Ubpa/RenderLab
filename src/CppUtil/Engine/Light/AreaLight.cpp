#include <CppUtil/Engine/AreaLight.h>

using namespace CppUtil::Engine;
using namespace glm;

vec3 AreaLight::Sample_L(const vec3 & p, vec3 & wi, float & distToLight, float & PD) const {
	if (p.z >= 0) {
		PD = 0;
		return vec3(0);
	}

	vec2 sample = vec2(sampler.GetSample() - vec2(0.5f, 0.5f));
	vec3 posOnLight(sample.x*width / 2, sample.y*height / 2, 0);

	vec3 d = posOnLight - p;
	float sqDist = dot(d, d);
	float dist = sqrt(sqDist);
	float cosTheta = d.z / dist;

	wi = d / dist;
	distToLight = dist;
	float area = width * height;
	PD = sqDist / (area * cosTheta);
	return intensity * color;
}

float AreaLight::PDF(const vec3 & p, const vec3 & wi) const {
	if (p.z >= 0)
		return 0;

	float zStep = p.z / wi.z;
	float x = p.x - wi.x * zStep;
	if (x >= width / 2 || x <= -width / 2)
		return 0;

	float y = p.y - wi.y * zStep;
	if (y >= height / 2 || y <= -height / 2)
		return 0;
	
	vec3 posOnLight(x, y, 0);
	vec3 d = posOnLight - p;
	float sqDist = dot(d, d);
	float dist = sqrt(sqDist);
	float cosTheta = d.z / dist;
	float area = width * height;
	return sqDist / (area * cosTheta);
}
