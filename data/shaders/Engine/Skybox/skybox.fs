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

vec3 GetEnvironment(vec3 texcoord);

void main()
{    
    vec3 ambient = GetEnvironment(TexCoords);
	
	// gamma
	FragColor = vec4(sqrt(ambient), 1);
}

vec3 GetEnvironment(vec3 texcoord) {
	if(!haveEnvironment)
		return vec3(0);
	
	if(!haveSkybox)
		return intensity * colorFactor;
	
	texcoord = - texcoord;
	float tmp = texcoord.x;
	texcoord.x = texcoord.z;
	texcoord.z = tmp;
	
	return intensity * colorFactor * texture(skybox, texcoord).rgb;
}
