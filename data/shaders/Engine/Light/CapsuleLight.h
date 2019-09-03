#ifndef ENGINE_LIGHT_CAPSULE_LIGHT_H
#define ENGINE_LIGHT_CAPSULE_LIGHT_H

#include "../../Math/line.h"
#include "../../Math/plane.h"

// ------------------------------ 接口 ------------------------------

// 48
struct CapsuleLight {
	vec3 p0;        // 12    0
	float radius;   //  4   12
	vec3 p1;        // 12   16
	vec3 L;         // 12   32
};

vec3 CapsuleLight_MRP(CapsuleLight light, vec3 vertex, vec3 R);

// ------------------------------ 实现 ------------------------------

vec3 CapsuleLight_MRP(CapsuleLight light, vec3 vertex, vec3 R) {
	vec3 axis = light.p1 - light.p0;
	float height2 = dot(axis, axis);
	float height = sqrt(height2);
	vec3 nAxis = axis / height;
	vec3 center = 0.5 * (light.p0 + light.p1);
	vec3 LtoV = vertex - center;
	vec3 n = normalize(LtoV - dot(LtoV, nAxis) * nAxis);

	vec3 fragToLight = -LtoV;

	// closest point
	// d0 : light dir
	// d1 : wi
	// d2 : R
	// d3 : a*d1 + b*d2
	// dot(d3, d0) = 0 --> b = - dot(d1, d0) * a / dot(d2, d0) = k * a
	//                 --> d3 = a * (d1 + k*d2)
	// dot(d3, d3) = r2 --> a2 = r2 / (d1 + k*d2)^2
	Line lineR = Line(vertex, R);
	Plane plane = Plane(center, n);
	vec3 intersectP = Intersect(lineR, plane);

	// get dist to border
	vec3 offset = intersectP - center;
	float offsetLen2 = dot(offset, offset);
	float cosTheta = dot(nAxis, offset / sqrt(offsetLen2));
	float cos2Theta = cosTheta * cosTheta;
	float sin2Theta = 1 - cos2Theta;
	float halfH = height / 2;
	float tanPhi = light.radius / halfH;
	float cos2Phi = 1 / (1 + tanPhi * tanPhi);
	float distToBorder2;
	if (cos2Theta > cos2Phi) {
		float r2 = light.radius * light.radius;
		float x = -halfH * sin2Theta + sqrt(cos2Theta*(r2 - halfH*halfH * sin2Theta));
		float distToBorder = (x + halfH) / cosTheta;
		distToBorder2 = distToBorder * distToBorder;
	}
	else {
		distToBorder2 = light.radius * light.radius / sin2Theta;
	}

	vec3 d0 = n;
	vec3 d1 = -normalize(fragToLight);
	vec3 d2 = R;

	float curR2 = saturate(offsetLen2 / distToBorder2) * distToBorder2;
	float k = -dot(d1, d0) / dot(d2, d0);
	vec3 d1kd2 = d1 + k * d2;
	float a2 = curR2 / dot(d1kd2, d1kd2);
	float a = sqrt(a2);
	vec3 d3 = a * d1kd2;

	return center + d3;
}

#endif // !ENGINE_LIGHT_CAPSULE_LIGHT_H
