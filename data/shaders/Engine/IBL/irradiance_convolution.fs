#version 330 core
out vec3 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
// ----------------------------------------------------------------------------
vec3 ImportanceSampleCosDir(vec2 u, vec3 N)
{
	// Local referencial
	vec3 upVector = abs(N.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
	vec3 tangentX = normalize(cross(upVector, N));
	vec3 tangentY = cross(N, tangentX);
	
	float u1 = u.x;
	float u2 = u.y;
	
	float r = sqrt(u1);
	float phi = u2 * PI * 2;
	
	vec3 L = vec3(r * cos(phi), r * sin(phi), sqrt(max(0.0f, 1.0f - u1)));
	L = normalize(tangentX * L.y + tangentY * L.x + N * L.z);
	
	return L;
}
// ----------------------------------------------------------------------------
void main()
{		
	// The world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    vec3 N = normalize(WorldPos);

    vec3 irradiance = vec3(0.0);
    
    const uint SAMPLE_NUM = 1024u;
	for(uint i=0u;i<SAMPLE_NUM;i++) {
		vec2 Xi = Hammersley(i, SAMPLE_NUM);
		vec3 L = ImportanceSampleCosDir(Xi, N);
		irradiance += texture(environmentMap, L).xyz;
	}
	
    irradiance *= 1.0 / float(SAMPLE_NUM);
    
    FragColor = irradiance;
}
