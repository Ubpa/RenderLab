#ifndef ENGINE_LIGHT_DISK_LIGHT_H
#define ENGINE_LIGHT_DISK_LIGHT_H

#include "../../Math/basic.h"
#include "../../Math/intersect.h"

// ------------------------------ 接口 ------------------------------

// 48
struct DiskLight {
	vec3 position;  // 12    0
	vec3 dir;       // 12   32
	vec3 luminance; // 12   16
	float radius;   //  4   44
};

vec3 DiskLight_MRP(DiskLight light, vec3 p, vec3 R);
vec3 DiskLight_Illuminance(DiskLight light, vec3 p, vec3 norm);

// ------------------------------ 实现 ------------------------------

vec3 DiskLight_MRP(DiskLight light, vec3 p, vec3 R) {
	// closest point
	// d0 : light dir
	// d1 : -wi
	// d2 : R
	// d3 : a*d1 + b*d2
	// dot(d3, d0) = 0 --> b = - dot(d1, d0) * a / dot(d2, d0) = k * a
	//                 --> d3 = a * (d1 + k*d2)
	// dot(d3, d3) = r2 --> a2 = r2 / (d1 + k*d2)^2
	Line lineR = Line(p, R);
	Plane planeDisk = Plane(light.position, light.dir);
	vec3 intersectP = Intersect(lineR, planeDisk);

	vec3 d0 = light.dir;
	vec3 d1 = normalize(p - light.position);
	vec3 d2 = R;
	vec3 offset = intersectP - light.position;
	float r2 = light.radius * light.radius;
	float curR2 = saturate(dot(offset, offset) / r2) * r2;
	float k = -dot(d1, d0) / dot(d2, d0);
	vec3 d1kd2 = d1 + k * d2;
	float a2 = curR2 / dot(d1kd2, d1kd2);
	float a = sqrt(a2);
	vec3 d3 = a * d1kd2;
	
	return light.position + d3;
}

vec3 DiskLight_Illuminance(DiskLight light, vec3 p, vec3 norm) {
	vec3 PtoL = light.position - p;
	float dist = length(PtoL);
	vec3 wi = PtoL / dist;
	float cosTheta = dot(wi, norm);
	float sinTheta = sqrt(1 - cosTheta * cosTheta);
	float cotTheta = cosTheta / sinTheta;
	float ratio = dist / light.radius;
	float ratio2 = ratio * ratio;

	float illuminanceFactor;
	if (cotTheta > 1 / ratio)
		illuminanceFactor = cosTheta / (1 + ratio2);
	else {
		float x = sqrt(1 - ratio2 * cotTheta * cotTheta);

		illuminanceFactor = -ratio * x * sinTheta / (PI * (1 + ratio2)) +
			(1 / PI) * atan(x * sinTheta / ratio) +
			cosTheta * (PI - acos(ratio * cotTheta)) / (PI * (1 + ratio2));
	}
	illuminanceFactor *= PI * dot(light.dir, -wi);
	illuminanceFactor = max(0.0, illuminanceFactor);

	return illuminanceFactor * light.luminance;
}

#endif // !ENGINE_LIGHT_DISK_LIGHT_H
