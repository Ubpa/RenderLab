#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

// 144
layout (std140) uniform Camera{
	mat4 view;			// 64	0
	mat4 projection;	// 64	64
	vec3 viewPos;		// 12	128
};

uniform mat4 model;

out vec2 TexCoords;

void main(){
	TexCoords = aTexCoords;

	gl_Position = projection * view * model * vec4(aPos,1.0);
}