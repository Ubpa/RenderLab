#version 330 core

#include "camera.h"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;

out vec2 TexCoords;

void main(){
	TexCoords = aTexCoords;

	gl_Position = projection * view * model * vec4(aPos,1.0);
}