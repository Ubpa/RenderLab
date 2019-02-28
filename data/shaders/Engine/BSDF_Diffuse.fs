#version 330 core
out vec4 FragColor;

struct BSDF_Diffuse {
	vec3 albedoColor;
	
	bool haveAlbedoTexture;
    sampler2D albedoTexture;
};

// 32
struct PointLight {
    vec3 position;	// 12
    vec3 L;			// 12
    float linear;	// 4
    float quadratic;// 4
};

#define MAX_POINT_LIGHTS 32
const float PI = 3.1415926;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

// 1040
layout (std140) uniform PointLights{
	int numLight;// 4
	PointLight pointLights[MAX_POINT_LIGHTS];// 32 * MAX_POINT_LIGHTS = 32 * 32
};

uniform BSDF_Diffuse bsdf;

void main()
{
	vec3 albedo = bsdf.albedoColor / PI;
	if(bsdf.haveAlbedoTexture){
		albedo *= texture(bsdf.albedoTexture,fs_in.TexCoords).xyz;
	}
	
	vec3 result = vec3(0);
    for(int i = 0; i < numLight; i++){
		vec3 dirToLight = pointLights[i].position - fs_in.FragPos;
		float dist2 = dot(dirToLight, dirToLight);
		float dist = sqrt(dist2);

		float cosTheta = max(dot(dirToLight/dist, normalize(fs_in.Normal)), 0);
		
		float attenuation = 1.0f + pointLights[i].linear * dist + pointLights[i].quadratic * dist2;
		
		result += cosTheta / attenuation * albedo * pointLights[i].L ;
	}
	
    FragColor = vec4(sqrt(result),1.0);
}
