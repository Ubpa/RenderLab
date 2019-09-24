#ifndef ENGINE_LIGHT_LTC
#define ENGINE_LIGHT_LTC

#include "AreaLight.h"

const float LUT_SIZE  = 64.0;
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;

// ------------------------------ 配置 ------------------------------

// GGX m(0,0) m(2,0) m(0,2) m(2,2)
uniform sampler2D LTCTex1;

// GGX norm, fresnel, 0(unused), sphere
uniform sampler2D LTCTex2;

// ------------------------------ 接口 ------------------------------
vec3 LTC_Spec(vec3 N, vec3 V, vec3 P, vec3 F0, float roughness, AreaLight light);
vec3 LTC_Diffuse(vec3 N, vec3 V, vec3 P, float roughness, AreaLight light);

// ------------------------------ 实现 ------------------------------

vec2 _LTC_CorrectUV(vec2 uv) {
	return uv * LUT_SCALE + LUT_BIAS;
}

// _LTC_GetInvM_GGX, _LTC_GetNF_GGX
vec2 _LTC_GetUV_GGX(float roughness, float NoV) {
	vec2 uv = vec2(roughness, sqrt(1.0 - NoV));
	return _LTC_CorrectUV(uv);
}

// GGX inv M
mat3 _LTC_GetInvM_GGX(vec2 uvGGX) {
	vec4 t1 = texture(LTCTex1, uvGGX);
	return mat3(
		vec3(t1.x,    0, t1.y), // col0
		vec3(   0,    1,    0), // col1
		vec3(t1.z,    0, t1.w)  // col2
	);
}

// GGX norm, Fresnel
vec2 _LTC_GetNF_GGX(vec2 uvGGX) {
	vec4 t2 = texture(LTCTex2, uvGGX);
	return vec2(t2.x, t2.y);
}

// projected solid angle of a spherical cap, clipped to the horizon
float _LTC_GetSphere(float NoF, float lenF) {
	vec2 uv = vec2(NoF * 0.5 + 0.5, lenF);
	uv = _LTC_CorrectUV(uv);

	return lenF * texture(LTCTex2, uv).w;
}

vec3 _LTC_IntegrateEdgeVec(vec3 v1, vec3 v2) {
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

float _LTC_IntegrateEdge(vec3 v1, vec3 v2) {
    return _LTC_IntegrateEdgeVec(v1, v2).z;
}

float _LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 invM, AreaLight light) {
	vec3 p0, p1, p2, p3;
	AreaLight_Corners(light, p0, p1, p2, p3);
	
	// construct orthonormal basis around N
	vec3 T1, T2;
	T1 = normalize(V - N*dot(V, N));
	T2 = cross(N, T1);

	// rotate area light in (T1, T2, N) basis
	invM = invM * transpose(mat3(T1, T2, N));

	// polygon
	vec3 L[4];
	L[0] = invM * (p0 - P);
	L[1] = invM * (p1 - P);
	L[2] = invM * (p2 - P);
	L[3] = invM * (p3 - P);

	// integrate
	float sum = 0.0;

	vec3 dir = p0.xyz - P;
	vec3 lightNormal = cross(p1 - p0, p3 - p0);
	bool behind = (dot(dir, lightNormal) > 0.0);

	L[0] = normalize(L[0]);
	L[1] = normalize(L[1]);
	L[2] = normalize(L[2]);
	L[3] = normalize(L[3]);

	vec3 vsum = vec3(0.0);

	vsum += _LTC_IntegrateEdgeVec(L[1], L[0]);
	vsum += _LTC_IntegrateEdgeVec(L[2], L[1]);
	vsum += _LTC_IntegrateEdgeVec(L[3], L[2]);
	vsum += _LTC_IntegrateEdgeVec(L[0], L[3]);

	float len = length(vsum);
	float z = vsum.z/len;

	if (behind)
		z = -z;
	
	sum = _LTC_GetSphere(z, len);

	if (behind)
		sum = 0.0;

	return sum;
}

vec3 LTC_Spec(vec3 N, vec3 V, vec3 P, vec3 F0, float roughness, AreaLight light) {
	vec2 uv = _LTC_GetUV_GGX(roughness, dot(N, V));
	
	mat3 invM = _LTC_GetInvM_GGX(uv);
	vec2 nf = _LTC_GetNF_GGX(uv);
	
	float spec = _LTC_Evaluate(N, V, P, invM, light);
	vec3 Fr = F0 * nf.x + (1 - F0) * nf.y;
	return light.luminance * spec * Fr;
}


vec3 LTC_Diffuse(vec3 N, vec3 V, vec3 P, float roughness, AreaLight light){
	vec2 uv = _LTC_GetUV_GGX(roughness, dot(N, V));
	
	mat3 invM = mat3(1);
	
	float albedo = _LTC_Evaluate(N, V, P, invM, light);
	return light.luminance * albedo;
}

#endif// !ENGINE_LIGHT_LTC
