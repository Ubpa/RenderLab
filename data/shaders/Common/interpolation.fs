#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform float weight[5] = float[] (0.4714, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	vec3 result = vec3(0);
	 
	result = texture(image, TexCoords).rgb;
	if(result != vec3(0)){
		FragColor = vec4(result,1.0f);
	}else{
		vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
		float weightSum = 0;
		for(int i=0;i<9;i++){
            for(int j=0;j<9;j++){
				vec3 tmpColor;
				tmpColor = texture(image, TexCoords + vec2(tex_offset.x * (i - 4), tex_offset.y * (j - 4))).rgb;
				if(tmpColor != vec3(0)){
					float w = weight[abs(i-4)]*weight[abs(j-4)];
					result += tmpColor * w;
					weightSum += w;
				}
			}
		}
		FragColor = vec4(result / weightSum, 1.0);
	}
}