#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform bool needTonemap = false;
uniform bool needGamma = false;

void main(){
	vec3 color = texture(image, TexCoords).rgb;
	
	
	// HDR tonemapping
	if(needTonemap)
		color = color / (vec3(1.0)+color);
	
	// gamma
	if(needGamma)
		color = pow(color, vec3(1.0/2.2));
	
	FragColor = vec4(color, 1.0f);
}
