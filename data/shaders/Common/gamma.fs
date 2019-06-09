#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

void main(){
	vec3 color = texture(image, TexCoords).rgb;
	color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color, 1.0f);
}