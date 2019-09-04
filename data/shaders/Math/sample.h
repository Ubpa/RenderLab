#ifndef MATH_SAMPLE_H
#define MATH_SAMPLE_H

#include "basic.h"
#include "../Math/quat.h"

// ------------------------------ 配置 ------------------------------

uniform float gTime = 1;

// ------------------------------ 接口 ------------------------------

void SampleInit(vec2 uv);

// [0, 1)，伪随机数，每次结果固定，相当于随机的映射函数
float PRNG(vec2 uv);

// [0, 1)，每次调用结果都不同，且与时间有关
float Rand(vec2 uv);

// [0, 1) x [0, 1)
vec2 UniformInSquare();

// center (0, 0), radius 1
vec2 UniformOnDisk(float Xi);
vec2 UniformOnDisk();

// center (0, 0), radius 1
vec2 UniformInDisk(vec2 Xi);
vec2 UniformInDisk();

// center (0, 0, 0), radius 1
vec3 UniformOnSphere(vec2 Xi);
vec3 UniformOnSphere();

// center (0, 0, 0), radius 1, z > 0
vec3 CosOnHalfSphere(vec2 Xi);
vec3 CosOnHalfSphere();

// center (0, 0, 0), radius 1, dir is n
vec3 CosOnHalfSphere(vec2 Xi, vec3 N);
vec3 CosOnHalfSphere(vec3 N);

// low discrepancy 2D random number, [0, 1) x [0, 1)
vec2 Hammersley(uint i, uint N);

// ------------------------------ 实现 ------------------------------

vec2 _uv = vec2(0);
void SampleInit(vec2 uv) {
	_uv = uv;
}

float PRNG() {
	return fract(sin(dot(_uv, vec2(12.9898, 78.233))) * 43758.5453);
}

float _Rand_Last = 0;
float Rand() {
	_Rand_Last = PRNG(_uv + vec2(_Rand_Last + gTime, _Rand_Last * gTime));
	return _Rand_Last;
}

vec2 UniformInSquare() {
	return vec2(Rand(), Rand());
}

vec2 UniformOnDisk() {
	return UniformOnDisk(Rand());
}

vec2 UniformOnDisk(float Xi) {
	float theta = TWO_PI * Xi;
	return vec2(cos(theta), sin(theta));
}

vec2 UniformInDisk(vec2 Xi) {
	float r = sqrt(Xi.x);
	return r * UniformOnDisk(Xi.y);
}

vec3 UniformOnSphere(vec2 Xi) {
	float t = 2 * sqrt(Xi.x * (1 - Xi.x));
	float phi = TWO_PI * Xi.y;

	float x = cos(phi) * t;
	float y = sin(phi) * t;
	float z = 1 - 2 * Xi.x;

	return vec3(x, y, z);
}

vec3 UniformOnSphere() {
	return UniformOnSphere(UniformInSquare());
}

vec3 CosOnHalfSphere(vec2 Xi) {
	float r = sqrt(Xi.x);
	vec2 pInDisk = r * UniformOnDisk(Xi.y);
	float z = sqrt(1 - Xi.x);
	return vec3(pInDisk, z);
}

vec3 CosOnHalfSphere() {
	return CosOnHalfSphere(UniformInSquare());
}

vec3 CosOnHalfSphere(vec2 Xi, vec3 N) {
	vec3 p = CosOnHalfSphere(Xi);
	vec4 rot = Quat_ZTo(N);
	return Quat_Rotate(rot, p);
}

vec3 CosOnHalfSphere(vec3 N) {
	return CosOnHalfSphere(UniformInSquare(), N);
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
