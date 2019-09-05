#ifndef ENGINE_LIGHT_POINT_LIGHT_H
#define ENGINE_LIGHT_POINT_LIGHT_H

#include "../../Math/basic.h"

// ------------------------------ 接口 ------------------------------

// 32
struct PointLight {
	vec3 position;          // 12   0
	float radius;           //  4  12
	vec3 illuminancePower;  // 12  16
};

vec3 PointLight_Illuminance(PointLight light, vec3 pos);

// ------------------------------ 实现 ------------------------------

vec3 PointLight_Illuminance(PointLight light, vec3 pos) {
	vec3 PtoL = light.position - pos;
	float dist2 = dot(PtoL, PtoL);
	float dist = sqrt(dist2);

	float falloff = Fwin(dist, light.radius);
	float attenuation = falloff / max(0.0001, dist2);
	vec3 illuminanceIntensity = light.illuminancePower * INV_FOUR_PI;

	return attenuation * illuminanceIntensity;
}

#endif //!ENGINE_LIGHT_POINT_LIGHT_H
