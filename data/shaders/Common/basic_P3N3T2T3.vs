#version 330 core

#include "camera.h"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
} vs_out;

uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz / worldPos.w;
    vs_out.TexCoords = aTexCoords;
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	
    vec3 N = normalize(normalMatrix * aNormal);
	vs_out.Normal = N;
	
    vec3 T = normalize(normalMatrix * aTangent);
    vs_out.Tangent = normalize(T - dot(T, N) * N);

    gl_Position = projection * view * worldPos;
}
