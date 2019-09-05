#ifndef MATH_BASIC_H
#define MATH_BASIC_H

#define PI            3.14159265359
#define TWO_PI        6.28318530718
#define FOUR_PI       12.56637061436
#define FOUR_PI2      39.47841760436
#define INV_PI        0.31830988618
#define INV_TWO_PI    0.15915494309
#define INV_FOUR_PI   0.07957747155
#define HALF_PI       1.57079632679
#define INV_HALF_PI   0.636619772367

// ------------------------------ 接口 ------------------------------

float saturate(float x);

// 0->R, 1->0
float Fwin(float x, float R);

// ------------------------------ 实现 ------------------------------

float saturate(float x) {
	return clamp(x, 0, 1);
}

float Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = max(0, 1 - ratio4);
	return falloff * falloff;
}

#endif // !MATH_BASIC_H
