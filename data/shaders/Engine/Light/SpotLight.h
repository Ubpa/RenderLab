#ifndef ENGINE_LIGHT_SPOT_LIGHT_H
#define ENGINE_LIGHT_SPOT_LIGHT_H

#include "../../Math/basic.h"

// ------------------------------ 接口 ------------------------------

// 112
struct SpotLight {
	vec3 position;         // 12      0
	float radius;          //  4     12
	vec3 dir;              // 12     16
	float cosHalfAngle;    //  4     28
	vec3 illuminancePower; // 12     32
	float cosFalloffAngle; //  4     44
	mat4 ProjView;         // 64     48
};

vec3 SpotLight_Illuminance(SpotLight light, vec3 pos);

// ------------------------------ 实现 ------------------------------

float _SpotLight_DirFalloff(SpotLight light, vec3 wi) {
	float cosTheta = -dot(wi, light.dir);
	if (cosTheta < light.cosHalfAngle) return 0;
	if (cosTheta > light.cosFalloffAngle) return 1;

	float t = (cosTheta - light.cosHalfAngle) /
		(light.cosHalfAngle - light.cosFalloffAngle);
	float t2 = t * t;
	return t2 * t2;
}

vec3 SpotLight_Illuminance(SpotLight light, vec3 pos) {
	vec3 PtoL = light.position - pos;
	float dist2 = dot(PtoL, PtoL);
	float dist = sqrt(dist2);
	vec3 wi = PtoL / dist;

	float distFalloff = Fwin(dist, light.radius);
	float dirFalloff = _SpotLight_DirFalloff(light, wi);
	float falloff = dirFalloff * distFalloff;

	float attenuation = falloff / max(0.0001, dist2);

	return attenuation * light.illuminancePower;
}

#endif //!ENGINE_LIGHT_SPOT_LIGHT_H
