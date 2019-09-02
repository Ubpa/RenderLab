#ifndef BRDF_DIFFUSE_H
#define BRDF_DIFFUSE_H

#include "../../Math/basic.h"

// ------------------------------ 接口 ------------------------------

void BRDF_Diffuse(out vec3 fd, out vec3 fs, vec3 albedo);
vec3 BRDF_Diffuse(vec3 albedo);

// ------------------------------ 实现 ------------------------------

void BRDF_Diffuse(out vec3 fd, out vec3 fs, vec3 albedo) {
	fd = albedo / PI;
	fs = vec3(0);
}

vec3 BRDF_Diffuse(vec3 albedo) {
	return albedo / PI;
}

#endif // !BRDF_DIFFUSE_H
