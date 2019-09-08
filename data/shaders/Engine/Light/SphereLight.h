#ifndef ENGINE_LIGHT_SPHERE_LIGHT_H
#define ENGINE_LIGHT_SPHERE_LIGHT_H

#include "../../Math/basic.h"

// ------------------------------ 接口 ------------------------------

// 32
struct SphereLight {
	vec3 position;  // 12    0
	vec3 luminance; // 12   16
	float radius;   //  4   28
};

vec3 SphereLight_Illuminance(SphereLight light, vec3 vertex, vec3 norm);
vec3 SphereLight_LuminancePower(SphereLight light);
vec3 SphereLight_MRP(SphereLight light, vec3 vertex, vec3 R);
float SphereLight_FsNormFactor(SphereLight light, vec3 vertex, float roughness);

// ------------------------------ 实现 ------------------------------

vec3 SphereLight_Illuminance(SphereLight light, vec3 vertex, vec3 norm) {
	vec3 diff = light.position - vertex;
	float H = length(diff);
	vec3 wi = diff / H;
	float cosBeta = dot(wi, norm);

	float h = H / light.radius;
	float h2 = h * h;

	float formFactor;
	if (h * cosBeta > 1)
		formFactor = cosBeta / h2;
	else {
		float sinBeta = sqrt(1 - cosBeta * cosBeta);
		float cotBeta = cosBeta / sinBeta;

		float x = sqrt(h2 - 1);
		float y = -x * cotBeta;

		formFactor = (1 / (PI * h2)) *
			(cosBeta * acos(y) - x * sinBeta * sqrt(1 - y * y)) +
			(1 / PI) * atan(sinBeta * sqrt(1 - y * y) / x);
	}
	formFactor = max(0.0, formFactor);

	return PI * formFactor * light.luminance;
}

vec3 SphereLight_LuminancePower(SphereLight light) {
	return FOUR_PI2 * light.radius * light.radius * light.luminance;
}

vec3 SphereLight_MRP(SphereLight light, vec3 vertex, vec3 R) {
	vec3 fragToLight = light.position - vertex;
	vec3 LtoR = dot(fragToLight, R) * R - fragToLight;
	return light.position + saturate(light.radius / length(LtoR)) * LtoR;
}

float SphereLight_FsNormFactor(SphereLight light, vec3 P, float roughness) {
	float dist = length(light.position - P);
	float alpha = roughness * roughness;
	float alphaPrime = saturate(alpha + light.radius / (3 * dist));
	float ratio = alpha / alphaPrime;
	return ratio * ratio;
}

#endif // !ENGINE_LIGHT_SPHERE_LIGHT_H
