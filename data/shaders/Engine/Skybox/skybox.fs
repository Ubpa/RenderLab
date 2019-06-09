#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// 32
layout (std140) uniform Environment{
	vec3 colorFactor;     // 12     0
	float intensity;      //  4    12
	bool haveSkybox;      //  4    16
	bool haveEnvironment; //  4    20
};

uniform samplerCube skybox;
uniform bool needGamma = false;

vec3 GetEnvironment(vec3 texcoord);

void main()
{    
    vec3 ambient = GetEnvironment(TexCoords);
	
	// gamma
	if(needGamma)
		ambient = pow(ambient, vec3(1.0/2.2));
	
	FragColor = vec4(ambient, 1);
}

vec3 GetEnvironment(vec3 texcoord) {
	if(!haveEnvironment)
		return vec3(0);
	
	if(!haveSkybox)
		return intensity * colorFactor;
	
	return intensity * colorFactor * texture(skybox, texcoord).rgb;
}
