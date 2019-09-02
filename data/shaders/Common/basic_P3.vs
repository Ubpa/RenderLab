#version 330 core

#include "camera.h"

layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main(){
	gl_Position = projection * view * model * vec4(aPos,1.0);
}