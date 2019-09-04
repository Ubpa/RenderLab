#version 330 core

#include "../../Math/sample.h"
#include "../BRDF/FDG.h"
#include "../../Math/quat.h"

out vec3 FragColor;
in vec2 TexCoords;

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
			vec3 H = SchlickGGX_Sample(Xi, N, roughness);
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
			if(NoL > 0) {
				C += DisneyDiffuseFr(N, V, L, sqrt(roughness));
			}
		}
    }

    return vec3(A, B, C) / float(SAMPLE_COUNT);
}
// ----------------------------------------------------------------------------
void main() 
{
	FragColor = IntegrateBRDF(TexCoords.x, TexCoords.y);
}
