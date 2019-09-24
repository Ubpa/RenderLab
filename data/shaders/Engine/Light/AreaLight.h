#ifndef ENGINE_LIGHT_AREA_LIGHT_H
#define ENGINE_LIGHT_AREA_LIGHT_H

#include "../../Math/line.h"
#include "../../Math/plane.h"

// ------------------------------ 接口 ------------------------------

// 64
struct AreaLight {
	vec3 position;  // 12    0
	float width;    // 12   12
	vec3 dir;       // 12   16
	float height;   //  4   28
	vec3 horizontal;// 12   32
	vec3 luminance; // 12   48
};

vec3 AreaLight_Illuminance(AreaLight light, vec3 vertex, vec3 norm);

vec3 AreaLight_MRP(AreaLight light, vec3 vertex, vec3 R);

void AreaLight_Corners(AreaLight light, out vec3 p0, out vec3 p1, out vec3 p2, out vec3 p3);

// ------------------------------ 实现 ------------------------------

// p0 BL, p1 BR, p2 TR, p3 TL
void AreaLight_Corners(AreaLight light, out vec3 p0, out vec3 p1, out vec3 p2, out vec3 p3) {
	float a = light.width * 0.5;
	float b = light.height * 0.5;
	vec3 verticle = cross(light.dir, light.horizontal);
	vec3 halfWidthVec = light.horizontal * a;
	vec3 halfHeightVec = verticle * b;

	p0 = light.position - halfHeightVec - halfWidthVec;
	p1 = light.position - halfHeightVec + halfWidthVec;
	p2 = light.position + halfHeightVec + halfWidthVec;
	p3 = light.position + halfHeightVec - halfWidthVec;
}

vec3 AreaLight_Illuminance(AreaLight light, vec3 vertex, vec3 norm) {
	vec3 fragToLight = light.position - vertex;
	if (dot(-fragToLight, light.dir) <= 0)
		return vec3(0);

	float dist2 = dot(fragToLight, fragToLight);
	float dist = sqrt(dist2);
	vec3 wi = fragToLight / dist;

	// rightPyramidSolidAngle
	float a = light.width * 0.5;
	float b = light.height * 0.5;
	float solidAngle = 4 * asin(a*b / sqrt((a*a + dist2) * (b*b + dist2)));
	
	vec3 p0, p1, p2, p3;
	AreaLight_Corners(light, p0, p1, p2, p3);

	float illuminanceFactor = solidAngle * 0.2 * (
		max(0, dot(normalize(p0 - vertex), norm)) +
		max(0, dot(normalize(p1 - vertex), norm)) +
		max(0, dot(normalize(p2 - vertex), norm)) +
		max(0, dot(normalize(p3 - vertex), norm)) +
		max(0, dot(wi, norm)));

	return illuminanceFactor * light.luminance;
}

vec3 AreaLight_MRP(AreaLight light, vec3 vertex, vec3 R) {
	vec3 fragToLight = light.position - vertex;

	// closest point
	// d0 : light dir
	// d1 : wi
	// d2 : R
	// d3 : a*d1 + b*d2
	// dot(d3, d0) = 0 --> b = - dot(d1, d0) * a / dot(d2, d0) = k * a
	//                 --> d3 = a * (d1 + k*d2)
	// dot(d3, d3) = r2 --> a2 = r2 / (d1 + k*d2)^2
	Line lineR = Line(vertex, R);
	Plane planeDisk = Plane(light.position, light.dir);
	vec3 intersectP = Intersect(lineR, planeDisk);

	// get dist to border
	vec3 offset = intersectP - light.position;
	float offsetLen2 = dot(offset, offset);
	float cosTheta = dot(light.horizontal, offset / sqrt(offsetLen2));
	float cos2Theta = cosTheta * cosTheta;
	float tanPhi = light.height / light.width;
	float cos2Phi = 1 / (1 + tanPhi * tanPhi);
	float distToBorder2;
	if (cos2Theta > cos2Phi) {
		float distToBorder = light.width / 2 / cosTheta;
		distToBorder2 = distToBorder * distToBorder;
	}
	else {
		float sin2Theta = 1 - cos2Theta;
		distToBorder2 = light.height * light.height / 4 / sin2Theta;
	}

	vec3 d0 = light.dir;
	vec3 d1 = -normalize(fragToLight);
	vec3 d2 = R;

	float curR2 = saturate(offsetLen2 / distToBorder2) * distToBorder2;
	float k = -dot(d1, d0) / dot(d2, d0);
	vec3 d1kd2 = d1 + k * d2;
	float a2 = curR2 / dot(d1kd2, d1kd2);
	float a = sqrt(a2);
	vec3 d3 = sign(dot(-R, d0)) * a * d1kd2;

	return light.position + d3;
}

#endif // !ENGINE_LIGHT_AREA_LIGHT_H
