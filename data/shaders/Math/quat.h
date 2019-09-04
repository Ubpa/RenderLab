#ifndef MATH_QUAT_H
#define MATH_QUAT_H

#include "basic.h"

// ------------------------------ 接口 ------------------------------

// axis is normalized
vec4 Quat(vec3 axis, float theta);

// from and to are normalized, from != -to
vec4 Quat_FromTo(vec3 from, vec3 to);
// from = (1, 0, 0), to is normalized, from != -to, faster than Quat_FromTo
vec4 Quat_XTo(vec3 to);
// from = (0, 1, 0), to is normalized, from != -to, faster than Quat_FromTo
vec4 Quat_YTo(vec3 to);
// from = (0, 0, 1), to is normalized, from != -to, faster than Quat_FromTo
vec4 Quat_ZTo(vec3 to);

// q is normalized
vec4 Quat_Inverse(vec4 q);

vec4 Quat_Mul(vec4 lhs, vec4 rhs);

vec3 Quat_Rotate(vec4 q, vec3 p);

// ------------------------------ 实现 ------------------------------

vec4 Quat(vec3 axis, float theta) {
	float halfTheta = theta / 2;
	return vec4(sin(halfTheta) * axis, cos(halfTheta));
}

vec4 Quat_FromTo(vec3 from, vec3 to) {
	float cosTheta = dot(from, to);
	float cosHalfTheta = sqrt(max(0, (cosTheta + 1) * 0.5));
	vec3 axisSinTheta = cross(from, to);
	return vec4(axisSinTheta / (2 * cosHalfTheta), cosHalfTheta);
}

vec4 Quat_XTo(vec3 to) {
	// from = (1, 0, 0)
	//float cosTheta = dot(from, to);
	//float cosHalfTheta = sqrt(max(0, (cosTheta + 1) * 0.5));
	float cosHalfTheta = sqrt(max(0, (to.x + 1) * 0.5));
	//vec3 axisSinTheta = cross(from, to);
	//    1    0    0
	// to.x to.y to.z
	//vec3 axisSinTheta = vec3(0, -to.z, to.y);
	float twoCosHalfTheta = 2 * cosHalfTheta;
	return vec4(0, -to.z / twoCosHalfTheta, to.y / twoCosHalfTheta, cosHalfTheta);

}

vec4 Quat_YTo(vec3 to) {
	// from = (0, 1, 0)
	//float cosTheta = dot(from, to);
	//float cosHalfTheta = sqrt(saturate((cosTheta + 1) * 0.5));
	float cosHalfTheta = sqrt(max(0, (to.y + 1) * 0.5));
	//vec3 axisSinTheta = cross(from, to);
	//    0    1    0
	// to.x to.y to.z
	//vec3 axisSinTheta = vec3(to.z, 0, - to.x);
	float twoCosHalfTheta = 2 * cosHalfTheta;
	return vec4(to.z / twoCosHalfTheta, 0, - to.x / twoCosHalfTheta, cosHalfTheta);
}

vec4 Quat_ZTo(vec3 to) {
	// from = (0, 0, 1)
	//float cosTheta = dot(from, to);
	//float cosHalfTheta = sqrt(max(0, (cosTheta + 1) * 0.5));
	float cosHalfTheta = sqrt(max(0, (to.z + 1) * 0.5));
	//vec3 axisSinTheta = cross(from, to);
	//    0    0    1
	// to.x to.y to.z
	//vec3 axisSinTheta = vec3(-to.y, to.x, 0);
	float twoCosHalfTheta = 2 * cosHalfTheta;
	return vec4(-to.y / twoCosHalfTheta, to.x / twoCosHalfTheta, 0, cosHalfTheta);
}

vec4 Quat_Inverse(vec4 q) {
	return vec4(-q.xyz, q.w);
}

vec4 Quat_Mul(vec4 lhs, vec4 rhs) {
	return vec4(
		lhs.w * rhs.xyz + rhs.w * lhs.xyz + cross(lhs.xyz, rhs.xyz),
		lhs.w * rhs.w - dot(lhs.xyz, rhs.xyz)
	);
}

vec3 Quat_Rotate(vec4 q, vec3 p) {
	// Quat_Mul(Quat_Mul(q, vec4(p, 0)), Quat_Inverse(q)).xyz;

	vec4 qp = vec4(q.w * p + cross(q.xyz, p), - dot(q.xyz, p));
	vec4 invQ = Quat_Inverse(q);
	vec3 qpInvQ = qp.w * invQ.xyz + invQ.w * qp.xyz + cross(qp.xyz, invQ.xyz);
	return qpInvQ;
}

#endif // !MATH_QUAT_H
