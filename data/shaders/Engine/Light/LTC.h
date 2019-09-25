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
vec2 _LTC_GetUV(float roughness, float NoV) {
	vec2 uv = vec2(roughness, sqrt(1.0 - NoV));
	return _LTC_CorrectUV(uv);
}

// GGX inv M
mat3 _LTC_GetInvM_GGX(vec2 uv) {
	vec4 t1 = texture(LTCTex1, uv);
	return mat3(
		vec3(t1.x,    0, t1.y), // col0
		vec3(   0,    1,    0), // col1
		vec3(t1.z,    0, t1.w)  // col2
	);
}

// GGX norm, Fresnel
vec2 _LTC_GetNF_GGX(vec2 uv) {
	vec4 t2 = texture(LTCTex2, uv);
	return vec2(t2.x, t2.y);
}

float _LTC_FitSphereIntegral(float NoF, float lenF) {
	return max(0, (lenF * lenF + NoF) / (lenF + 1));
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
	
    return cross(v1, v2) * theta_sintheta;
}

float _LTC_IntegrateEdge(vec3 v1, vec3 v2) {
    return _LTC_IntegrateEdgeVec(v1, v2).z;
}

void _LTC_ClipQuadToHorizon(inout vec3 L[5], out int n) {
    // detect clipping config
    int config = 0;
    if (L[0].z > 0.0) config += 1;
    if (L[1].z > 0.0) config += 2;
    if (L[2].z > 0.0) config += 4;
    if (L[3].z > 0.0) config += 8;

    // clip
    n = 0;

    if (config == 0)
    {
        // clip all
    }
    else if (config == 1) // V1 clip V2 V3 V4
    {
        n = 3;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[3].z * L[0] + L[0].z * L[3];
    }
    else if (config == 2) // V2 clip V1 V3 V4
    {
        n = 3;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    }
    else if (config == 3) // V1 V2 clip V3 V4
    {
        n = 4;
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
        L[3] = -L[3].z * L[0] + L[0].z * L[3];
    }
    else if (config == 4) // V3 clip V1 V2 V4
    {
        n = 3;
        L[0] = -L[3].z * L[2] + L[2].z * L[3];
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
    }
    else if (config == 5) // V1 V3 clip V2 V4) impossible
    {
        n = 0;
    }
    else if (config == 6) // V2 V3 clip V1 V4
    {
        n = 4;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    }
    else if (config == 7) // V1 V2 V3 clip V4
    {
        n = 5;
        L[4] = -L[3].z * L[0] + L[0].z * L[3];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    }
    else if (config == 8) // V4 clip V1 V2 V3
    {
        n = 3;
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
        L[1] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] =  L[3];
    }
    else if (config == 9) // V1 V4 clip V2 V3
    {
        n = 4;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[2].z * L[3] + L[3].z * L[2];
    }
    else if (config == 10) // V2 V4 clip V1 V3) impossible
    {
        n = 0;
    }
    else if (config == 11) // V1 V2 V4 clip V3
    {
        n = 5;
        L[4] = L[3];
        L[3] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    }
    else if (config == 12) // V3 V4 clip V1 V2
    {
        n = 4;
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
    }
    else if (config == 13) // V1 V3 V4 clip V2
    {
        n = 5;
        L[4] = L[3];
        L[3] = L[2];
        L[2] = -L[1].z * L[2] + L[2].z * L[1];
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
    }
    else if (config == 14) // V2 V3 V4 clip V1
    {
        n = 5;
        L[4] = -L[0].z * L[3] + L[3].z * L[0];
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
    }
    else if (config == 15) // V1 V2 V3 V4
    {
        n = 4;
    }

    if (n == 3)
        L[3] = L[0];
    if (n == 4)
        L[4] = L[0];
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
	vec3 L[5];
	L[0] = invM * (p0 - P);
	L[1] = invM * (p1 - P);
	L[2] = invM * (p2 - P);
	L[3] = invM * (p3 - P);

	// integrate
	float sum = 0.0;

#if 0 // sphere approximation
	vec3 dir = p0.xyz - P;
	vec3 lightNormal = cross(p1 - p0, p3 - p0);
	bool behind = dot(dir, lightNormal) > 0.0;

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
#else
	int n;
	_LTC_ClipQuadToHorizon(L, n);

	if (n == 0)
		return 0;
	// project onto sphere
	L[0] = normalize(L[0]);
	L[1] = normalize(L[1]);
	L[2] = normalize(L[2]);
	L[3] = normalize(L[3]);
	L[4] = normalize(L[4]);

	// integrate
	sum += _LTC_IntegrateEdge(L[1], L[0]);
    sum += _LTC_IntegrateEdge(L[2], L[1]);
    sum += _LTC_IntegrateEdge(L[3], L[2]);
    if (n >= 4)
        sum += _LTC_IntegrateEdge(L[4], L[3]);
    if (n == 5)
        sum += _LTC_IntegrateEdge(L[0], L[4]);

    sum = max(0.0, sum);
#endif

	return sum;
}

vec3 LTC_Spec(vec3 N, vec3 V, vec3 P, vec3 F0, float roughness, AreaLight light) {
	float NoV = saturate(dot(N, V));
	vec2 uv = _LTC_GetUV(roughness, NoV);
	
	mat3 invM = _LTC_GetInvM_GGX(uv);
	vec2 nf = _LTC_GetNF_GGX(uv);
	
	vec3 Fr = F0 * nf.x + (1 - F0) * nf.y;
	vec3 L = light.luminance * Fr;
	
	float spec = _LTC_Evaluate(N, V, P, invM, light);
	return spec * L;
}

vec3 LTC_Diffuse(vec3 N, vec3 V, vec3 P, float roughness, AreaLight light){
	mat3 invM = mat3(1);
	
	float diffuse = _LTC_Evaluate(N, V, P, invM, light);
	return light.luminance * diffuse;
}

#endif// !ENGINE_LIGHT_LTC
