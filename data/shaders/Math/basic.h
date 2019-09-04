#ifndef MATH_BASIC_H
#define MATH_BASIC_H

#define PI            3.14159265359
#define TWO_PI        6.28318530718
#define FOUR_PI       12.56637061436
#define INV_PI        0.31830988618
#define INV_TWO_PI    0.15915494309
#define INV_FOUR_PI   0.07957747155
#define HALF_PI       1.57079632679
#define INV_HALF_PI   0.636619772367

// ------------------------------ 接口 ------------------------------

float saturate(float x);

// ------------------------------ 实现 ------------------------------

float saturate(float x) {
	return clamp(x, 0, 1);
}

#endif // !MATH_BASIC_H
