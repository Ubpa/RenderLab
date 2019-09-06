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
	float height;   //  4   28
	vec3 L;         // 12   32
};

float CapsuleLight_Area(CapsuleLight light);

vec3 CapsuleLight_MRP(CapsuleLight light, vec3 p, vec3 R);

float CapsuleLight_AvrgDist2(CapsuleLight light, vec3 p);

// ------------------------------ 实现 ------------------------------

float CapsuleLight_Area(CapsuleLight light) {
	float r2 = light.radius * light.radius;
	return FOUR_PI * r2 + TWO_PI * light.radius * light.height;
}

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

float CapsuleLight_AvrgDist2(CapsuleLight light, vec3 P) {
	vec3 PO = light.p0 - P;
	vec3 D = light.p1 - light.p0;

	float DoD = dot(D, D);
	float POoD = dot(PO, D);
	float POoPO = dot(PO, PO);

#if 0
	// mean square
	/* latex
	\begin{align}
	 & \int_0^1 (O + tD - P)^2 dt\\
	=& \int_0^1 D^2 t^2 + 2 D\cdot PO t + PO^2 dt\\
	=& D^2/3 t^3 + D\cdot PO t^2 + PO^2 t |_0^1\\
	=& D^2/3 + D\cdot PO + PO^2\\
	\end{align}
	*/
	
	return DoD / 3 + POoD + POoPO;
#else
	// inverse mean inverse square
	float a = DoD;
	float b = 2 * POoD;
	float c = POoPO;
	float p = b / a;
	float q = c / a;
	float w = sqrt(q - p * p / 4);
	float intRst = (atan((1 + p / 2) / w) - atan(p / (2 * w))) / (a*w);
	return 1 / intRst;
#endif
}

#endif // !ENGINE_LIGHT_CAPSULE_LIGHT_H
