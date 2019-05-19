#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

// 160
layout (std140) uniform Camera{
	mat4 view;			// 64	0	64
	mat4 projection;	// 64	64	64
	vec3 viewPos;		// 12	128	144
	float nearPlane;	// 4	144	148
	float farPlane;		// 4	148	152
	float fov;			// 4	152	156
	float ar;			// 4	156	160
};

void main()
{
    TexCoords = aPos;
	mat4 viewNoT = mat4(mat3(view));
    vec4 pos = projection * viewNoT * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
