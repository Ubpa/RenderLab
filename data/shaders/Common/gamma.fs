#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

void main(){
	FragColor = vec4(sqrt(texture(image, TexCoords).rgb), 1.0f);
}