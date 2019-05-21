#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

#define PI 3.14159265359

uniform sampler2D equirectangularMap;

float atan2(float y, float x);

vec2 TexcoordOf(vec3 dir);

void main()
{		
    vec2 uv = TexcoordOf(normalize(WorldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}

float atan2(float y, float x) {
    if(x > 0)
        return atan(y/x);
    else if(x < 0) {
        if(y >= 0)
            return atan(y/x) + PI;
        else
            return atan(y/x) - PI;
    }
    else // x == 0
        return sign(y) * PI / 2;
}

vec2 TexcoordOf(vec3 dir) {
	float phi = atan2(-dir.x, -dir.z) + PI;
	float theta = acos(dir.y);

	float u = phi / (2.f * PI);
	float v = theta / PI;
	return vec2(u,v);
}
