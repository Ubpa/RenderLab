#version 330 core

layout (location = 0) out vec4 GBuffer0;
layout (location = 1) out vec4 GBuffer1;
layout (location = 2) out vec4 GBuffer2;
layout (location = 3) out vec4 GBuffer3;

// layout
//      x   y   z   w
// 0 [   pos    ]  ID
// 1 [   norm   ]
// 2 [    L     ]
// 3 

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
} fs_in;

struct BSDF_Emission {
	vec3 L;
};

uniform BSDF_Emission emission;
uniform int ID;

void main() {
	// pack GBuffer
	GBuffer0 = vec4(fs_in.FragPos, ID);
	GBuffer1 = vec4(normalize(fs_in.Normal), 0);
	GBuffer2 = vec4(emission.L, 0);
	GBuffer3 = vec4(0);
}
