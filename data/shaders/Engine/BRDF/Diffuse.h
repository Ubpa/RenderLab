#ifndef BRDF_DIFFUSE_H
#define BRDF_DIFFUSE_H

#include "../../Math/basic.h"

// ------------------------------ 接口 ------------------------------

void BRDF_Diffuse(out vec3 fd, out vec3 fs, vec3 albedo);
vec3 BRDF_Diffuse(vec3 albedo);

// diffuse
vec3 BRDFd_Diffuse(vec3 albedo);
// specular
vec3 BRDFs_Diffuse(vec3 albedo);

// ------------------------------ 实现 ------------------------------

vec3 BRDFd_Diffuse(vec3 albedo) {
	return albedo * INV_PI;
}

vec3 BRDFs_Diffuse(vec3 albedo) {
	return vec3(0);
}

void BRDF_Diffuse(out vec3 fd, out vec3 fs, vec3 albedo) {
	fd = albedo * INV_PI;
	fs = vec3(0);
}

vec3 BRDF_Diffuse(vec3 albedo) {
	return albedo * INV_PI;
}

#endif // !BRDF_DIFFUSE_H
