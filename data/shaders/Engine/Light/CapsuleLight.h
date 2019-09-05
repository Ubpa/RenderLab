#ifndef ENGINE_LIGHT_CAPSULE_LIGHT_H
#define ENGINE_LIGHT_CAPSULE_LIGHT_H

#include "../../Math/line.h"
#include "../../Math/plane.h"
#include "../../Math/Segment.h"
#include "DiskLight.h"
#include "SphereLight.h"

// ------------------------------ 接口 ------------------------------

// 48
struct CapsuleLight {
	vec3 p0;        // 12    0
	float radius;   //  4   12
	vec3 p1;        // 12   16
	vec3 L;         // 12   32
};

vec3 CapsuleLight_MRP(CapsuleLight light, vec3 p, vec3 R);

// ------------------------------ 实现 ------------------------------

vec3 CapsuleLight_MRP(CapsuleLight light, vec3 p, vec3 R) {
	vec3 o1 = light.p0;
	vec3 d1 = light.p1 - light.p0;
	vec3 o2 = p;
	vec3 d2 = R;

	vec3 o12 = o2 - o1;

	float o12d1 = dot(o12, d1);
	float o12d2 = dot(o12, d2);
	float d1d1 = dot(d1, d1);
	float d1d2 = dot(d1, d2);
	float d2d2 = dot(d2, d2);

	float t = (o12d1 * d2d2 - o12d2 * d1d2) / (d1d1 * d2d2 - d1d2 * d1d2);
	vec3 closestP = o1 + saturate(t) * d1;

	SphereLight sphereLight = SphereLight(closestP, light.L, light.radius);
	return SphereLight_MRP(sphereLight, p, R);
}

#endif // !ENGINE_LIGHT_CAPSULE_LIGHT_H
