#ifndef MATH_SAMPLE_H
#define MATH_SAMPLE_H

#include "basic.h"

// ------------------------------ 接口 ------------------------------
uniform float gTime;

// ------------------------------ 接口 ------------------------------

void SampleInit(vec2 uv);

// [0, 1)，伪随机数，每次结果固定，相当于随机的映射函数
float PRNG(vec2 uv);

// [0, 1)，每次调用结果都不同，且与时间有关
float Rand(vec2 uv);

// [0, 1) x [0, 1)
vec2 UniformInSquare(vec2 uv);

// center (0, 0), radius 1
vec2 UniformOnDisk(vec2 uv);

// center (0, 0), radius 1
vec2 UniformInDisk(vec2 uv);

// center (0, 0, 0), radius 1
vec3 UniformOnSphere(vec2 uv);

// center (0, 0, 0), radius 1, z > 0
vec3 CosOnHalfSphere(vec2 uv);

// center (0, 0, 0), radius 1, dir is n
vec3 CosOnHalfSphere(vec2 uv, vec3 N);

// low discrepancy 2D random number, [0, 1) x [0, 1)
vec2 Hammersley(uint i, uint N);

// ------------------------------ 实现 ------------------------------

float PRNG(vec2 uv) {
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

float _Rand_Last = 0;
float Rand(vec2 uv) {
	_Rand_Last = PRNG(uv + vec2(_Rand_Last + gTime, _Rand_Last * gTime));
	return _Rand_Last;
}

vec2 UniformInSquare(vec2 uv) {
	return vec2(Rand(uv), Rand(uv));
}

vec2 UniformOnDisk(vec2 uv) {
	float theta = TWO_PI * Rand(uv);
	return vec2(cos(theta), sin(theta));
}

vec2 UniformInDisk(vec2 uv) {
	float r = sqrt(Rand(uv));
	return r * UniformOnDisk(uv);
}

vec3 UniformOnSphere(vec2 uv) {
	float Xi1 = Rand(uv);
	float Xi2 = Rand(uv);

	float phi = TWO_PI * Xi2;
	float t = 2 * sqrt(Xi1 * (1 - Xi1));

	float x = cos(phi) * t;
	float y = sin(phi) * t;
	float z = 1 - 2 * Xi1;

	return vec3(x, y, z);
}

vec3 CosOnHalfSphere(vec2 uv) {
	vec2 pInDisk = UniformInDisk(uv);
	float z = sqrt(1 - pInDisk.x * pInDisk.x - pInDisk.y * pInDisk.y);
	return vec3(pInDisk, z);
}

vec3 CosOnHalfSphere(vec2 uv, vec3 N) {
	// Local referencial
	vec3 upVector = abs(N.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
	vec3 tangentX = normalize(cross(upVector, N));
	vec3 tangentY = cross(N, tangentX);

	vec3 p = CosOnHalfSphere(uv);
	p = normalize(tangentX * p.y + tangentY * p.x + N * p.z);

	return p;
}

float _RadicalInverse_VdC(uint bits) {
	// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	// efficient VanDerCorpus calculation.

	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N) {
	return vec2(float(i) / float(N), _RadicalInverse_VdC(i));
}

#endif // !MATH_SAMPLE_H
