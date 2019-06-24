#version 330 core

layout (location = 0) out vec4 GBuffer0;
layout (location = 1) out vec4 GBuffer1;
layout (location = 2) out vec4 GBuffer2;
layout (location = 3) out vec4 GBuffer3;

// layout
//      x   y   z   w
// 0 [    pos   ]  ID
// 1 [   norm   ]   1
// 2 [  albedo  ]   0
// 3                1

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
} fs_in;

struct BSDF_Diffuse {
	vec3 colorFactor;
	bool haveAlbedoTexture;
    sampler2D albedoTexture;
};

uniform BSDF_Diffuse diffuse;
uniform int ID;

void main() {
	// 获取属性值
	vec3 albedo = diffuse.colorFactor;
	if(diffuse.haveAlbedoTexture) {
		albedo *= texture(diffuse.albedoTexture, fs_in.TexCoords).xyz;
	}
	
	// pack GBuffer
	GBuffer0 = vec4(fs_in.FragPos, ID);
	GBuffer1 = vec4(normalize(fs_in.Normal), 1);
	GBuffer2 = vec4(albedo, 0);
	GBuffer3 = vec4(0, 0, 0, 1);
}
