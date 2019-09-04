#version 330 core

#include "../../Math/sample.h"
#include "../BRDF/FDG.h"

out vec3 FragColor;
in vec2 TexCoords;

// ----------------------------------------------------------------------------
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness*roughness;
	
	float phi = TWO_PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// from tangent-space H vector to world-space sample vector
	vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}
// ----------------------------------------------------------------------------
vec3 IntegrateBRDF(float NdotV, float roughness)
{
	// 由于各向同性，随意取一个 V 即可
    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;
	float C = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);
    
    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
		
		{ // A and B
			vec3 H = ImportanceSampleGGX(Xi, N, roughness);
			vec3 L = normalize(2.0 * dot(V, H) * H - V);

			float NdotL = max(L.z, 0.0);
			float NdotH = max(H.z, 0.0);
			float VdotH = max(dot(V, H), 0.0);

			if(NdotL > 0.0)
			{
				float G = SchlickGGX_SmithG(N, V, L, roughness);
				float G_Vis = (G * VdotH) / (NdotH * NdotV);
				float Fc = pow(1.0 - VdotH, 5.0);

				A += (1.0 - Fc) * G_Vis;
				B += Fc * G_Vis;
			}
		}
		
		{// Disney Diffuse DFG
			vec3 L = CosOnHalfSphere(Xi, N);
			float NoL = dot(N, L);
			if(NoL > 0){
				float LdotH = clamp(dot(L, normalize (V + L)), 0, 1);
				float NdotV = clamp(dot(N, V), 0, 1);
				C += DisneyDiffuseFr(N, V, L, sqrt(roughness));
			}
		}
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    C /= float(SAMPLE_COUNT);
    return vec3(A, B, C);
}
// ----------------------------------------------------------------------------
void main() 
{
    vec3 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
    FragColor = integratedBRDF;
}