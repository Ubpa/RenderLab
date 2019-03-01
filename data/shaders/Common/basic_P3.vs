#version 330 core

layout (location = 0) in vec3 aPos;

// 144
layout (std140) uniform Camera{
	mat4 view;			// 64	0
	mat4 projection;	// 64	64
	vec3 viewPos;		// 12	128
};

uniform mat4 model;

void main(){
	gl_Position = projection * view * model * vec4(aPos,1.0);
}