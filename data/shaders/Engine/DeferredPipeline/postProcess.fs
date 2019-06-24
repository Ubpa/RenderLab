#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D img;

void main(){
	vec3 color = texture(img, TexCoords).xyz;
	
	// HDR tonemapping
	color = color / (vec3(1.0)+color);
	
	// gamma
	color = pow(color, vec3(1.0/2.2));
	
	FragColor = color;
}