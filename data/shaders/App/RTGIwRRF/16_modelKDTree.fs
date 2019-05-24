#version 330 core

// ----------------- in out

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
} fs_in;

// ----------------- const

#define MAX_POINT_LIGHTS 8
const float PI = 3.14159265359;
const float INV_PI = 0.31830988618;

// array of offset direction for sampling
const vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

// ----------------- struct

struct BSDF_FrostedGlass {
	vec3 colorFactor;
	bool haveColorTexture;
    sampler2D colorTexture;

	float roughnessFactor;
	bool haveRoughnessTexture;
    sampler2D roughnessTexture;
	
	bool haveAOTexture;
    sampler2D aoTexture;

	bool haveNormalTexture;
	sampler2D normalTexture;

	float ior;
};

// ----------------- Uniform

// 48
struct PointLight {
    vec3 position;	// 12	0
    vec3 L;			// 12	16
    float linear;	// 4	28
    float quadratic;// 4	32
};

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

// 400
layout (std140) uniform PointLights{
	int numLight;// 16
	PointLight pointLights[MAX_POINT_LIGHTS];// 48 * MAX_POINT_LIGHTS = 48 * 8
};

uniform BSDF_FrostedGlass bsdf;

uniform samplerCube pointLightDepthMap0;
uniform samplerCube pointLightDepthMap1;
uniform samplerCube pointLightDepthMap2;
uniform samplerCube pointLightDepthMap3;
uniform samplerCube pointLightDepthMap4;
uniform samplerCube pointLightDepthMap5;
uniform samplerCube pointLightDepthMap6;
uniform samplerCube pointLightDepthMap7;

uniform float lightFar;

uniform int mode;

uniform float interpolateRatio; // (0, 1]

// ----------------- declaration

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord);

float GGX_D(vec3 norm, vec3 h, float alpha);
float Fr(vec3 v, vec3 h, float ior);
float GGX_G1(vec3 norm, vec3 v, vec3 h, float alpha);
float GGX_G(vec3 norm, vec3 wo, vec3 wi, vec3 h, float alpha);
vec3 BSDF(vec3 norm, vec3 wo, vec3 wi, vec3 color, float roughness, float ao, float ior);

float Visibility(vec3 lightToFrag, int id);
float Visibility(vec3 lightToFrag, samplerCube depthMap);

float tanh(float x) {
	float expZ = exp(x);
    float invExpZ = 1 / expZ;
    return (expZ - invExpZ) / (expZ + invExpZ);
}

float smootherstep(float x){
	return ((6*x - 15)*x + 10) * x*x*x;
}

// template declaration
void Model
(
	in float directIllum_R,
	in float directIllum_G,
	in float directIllum_B,
	in float pos_x,
	in float pos_y,
	in float pos_z,
	in float viewDir_x,
	in float viewDir_y,
	in float viewDir_z,
	in float normal_x,
	in float normal_y,
	in float normal_z,
	in float matColor_R,
	in float matColor_G,
	in float matColor_B,
	in float IOR,
	in float roughness,
	
	out float indirectIllum_R,
	out float indirectIllum_G,
	out float indirectIllum_B
);

// ----------------- main

void main() {
	// get attr
	vec3 color = bsdf.colorFactor;
	if(bsdf.haveColorTexture) {
		color *= texture(bsdf.colorTexture, fs_in.TexCoords).xyz;
	}

	float roughness = bsdf.roughnessFactor;
	if(bsdf.haveRoughnessTexture) {
		roughness *= texture(bsdf.roughnessTexture, fs_in.TexCoords).x;
	}

	float ao = 1.0f;
	if(bsdf.haveAOTexture) {
		ao *= texture(bsdf.aoTexture, fs_in.TexCoords).x;
	}

	vec3 wo = normalize(viewPos - fs_in.FragPos);

	vec3 norm = normalize(fs_in.Normal);
	if(bsdf.haveNormalTexture) {
		norm = CalcBumpedNormal(norm, normalize(fs_in.Tangent), bsdf.normalTexture, fs_in.TexCoords);
	}
	
	// sample point light
	vec3 result = vec3(0);
    for(int i = 0; i < numLight; i++) {
		vec3 fragToLight = pointLights[i].position - fs_in.FragPos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight / dist;
		
		float visibility = Visibility(-fragToLight, i);
		if(visibility==0)
			continue;

		vec3 f = BSDF(norm, wo, wi, color, roughness, ao, bsdf.ior);

		float cosTheta = max(dot(wi, norm), 0);
		
		float attenuation = 1.0f + pointLights[i].linear * dist + pointLights[i].quadratic * dist2;
		
		result += visibility * cosTheta / attenuation * f * pointLights[i].L;
	}
	
	vec3 indirectIllum;
	
	Model
	(
		// input
		result.r,
		result.g,
		result.b,
		fs_in.FragPos.x,
		fs_in.FragPos.y,
		fs_in.FragPos.z,
		wo.x,
		wo.y,
		wo.z,
		norm.x,
		norm.y,
		norm.z,
		color.r,
		color.g,
		color.b,
		bsdf.ior,
		roughness,
		
		// output
		indirectIllum.r,
		indirectIllum.g,
		indirectIllum.b
	);
	
	indirectIllum = clamp(indirectIllum, 0, 1);
	
	// gamma correction
	vec3 finalColor;
	if(mode==0)
		finalColor = result;
	else if(mode==1)
		finalColor = indirectIllum;
	else
		finalColor = result + indirectIllum;
		
    FragColor = vec4(sqrt(finalColor), 1.0);
}

// ----------------- definition

float GGX_D(vec3 norm, vec3 h, float alpha) {
	float HoN = dot(h, norm);
	if (HoN <= 0)
		return 0;

	float cos2 = HoN * HoN;
	float alpha2 = alpha * alpha;

	float root = alpha / (cos2 * (alpha2 - 1) + 1);

	return INV_PI * root * root;
}

float Fr(vec3 v, vec3 h, float ior) {
	// angle in low ior media
	float cosTheta = dot(v, h);
	bool entering = cosTheta > 0.0f;
	
	if (!entering) {
		vec3 vInLowIOR = refract(-v, -h, ior);
		if (vInLowIOR == vec3(0))
			return 1;

		cosTheta = dot(vInLowIOR, h);
	}

	float R0 = pow((ior - 1) / (ior + 1), 2);

	float Fr = R0 + (1 - R0) * pow((1 - cosTheta), 5);

	return Fr;
}

float GGX_G1(vec3 norm, vec3 v, vec3 h, float alpha) {
	float NoV = dot(norm, v);
	float HoV = dot(v, h);
	if (NoV * HoV <= 0)
		return 0;

	float alpha2 = alpha * alpha;
	float tan2 = 1.f / (NoV * NoV) - 1.f;
	return 2.f / (1.f + sqrt(1 + alpha2 * tan2));
}

float GGX_G(vec3 norm, vec3 wo, vec3 wi, vec3 h, float alpha) {
	return GGX_G1(norm, wo, h, alpha) * GGX_G1(norm, wi, h, alpha);
}

vec3 BSDF(vec3 norm, vec3 wo, vec3 wi, vec3 color, float roughness, float ao, float ior) {
	float NoWo = dot(norm, wo);
	float NoWi = dot(norm, wi);
	if(NoWo == 0 || NoWi == 0)
		return vec3(0);

	bool entering = NoWo > 0;
	bool isReflect = NoWo * NoWi > 0;

	float alpha = roughness * roughness;

	if (isReflect) {
		vec3 h = normalize(wo + wi);

		h *= sign(dot(h, norm));// let h point to outside
		float bsdfVal = Fr(wo, h, ior) * GGX_D(norm, h, alpha) * GGX_G(norm, wo, wi, h, alpha) / abs(4.f * NoWo * NoWi);
		return bsdfVal * color;
	}
	else {
		float etai = 1.f, etat = ior;
		if (!entering){
			// swap(etai, etat);
			float tmp = etai;
			etai = etat;
			etat = tmp;
		}

		vec3 h = -normalize(etai * wo + etat * wi);

		float HoWo = dot(h, wo);
		float HoWi = dot(h, wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;

		float factor = abs(HoWo * HoWi / (NoWo * NoWi));

		h *= sign(dot(h,norm));// let h point to outside
		float bsdfVal = factor * ((1 - Fr(wo, h, ior)) * GGX_D(norm, h, alpha) * GGX_G(norm, wo, wi, h, alpha) * etat * etat) / (sqrtDenom * sqrtDenom);
		return bsdfVal * color;
	}
}

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord) {
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);
    vec3 bumpMapNormal = texture(normalTexture, texcoord).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - 1.0;
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec3 newNormal = TBN * bumpMapNormal;
    newNormal = normalize(newNormal);
    return newNormal;
}

float Visibility(vec3 lightToFrag, int id){
	if(id == 0) {
		return Visibility(lightToFrag, pointLightDepthMap0);
	} else if(id == 1) {
		return Visibility(lightToFrag, pointLightDepthMap1);
	} else if(id == 2) {
		return Visibility(lightToFrag, pointLightDepthMap2);
	} else if(id == 3) {
		return Visibility(lightToFrag, pointLightDepthMap3);
	} else if(id == 4) {
		return Visibility(lightToFrag, pointLightDepthMap4);
	} else if(id == 5) {
		return Visibility(lightToFrag, pointLightDepthMap5);
	} else if(id == 6) {
		return Visibility(lightToFrag, pointLightDepthMap6);
	} else if(id == 7) {
		return Visibility(lightToFrag, pointLightDepthMap7);
	}else 
		return 1;// not support id
}

float Visibility(vec3 lightToFrag, samplerCube depthMap) {
	float currentDepth = length(lightToFrag);
	float bias = 0.08;
	int samples = 20;
	float shadow = 0.0;
	float viewDistance = length(viewPos - fs_in.FragPos);
	float diskRadius = (1.0 + (viewDistance / lightFar)) / 50.0;
	for(int i = 0; i < samples; ++i) {
		float closestDepth = lightFar * texture(depthMap, lightToFrag + gridSamplingDisk[i] * diskRadius).r;
		shadow += smoothstep(closestDepth, closestDepth + bias, currentDepth);
	}
	shadow /= float(samples);
	return 1 - shadow;
}

void FNN_16_1_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(0.195313)+x1*(-0.0980876)+x2*(0.138492)+x3*(0.130111)+x4*(-0.173656)+x5*(0.207205)+x6*(0.233867)+x7*(0.0617001)+x8*(0.179805)+x9*(0.141699)+x10*(0.330886)+x11*(-0.272748)+x12*(-0.0397303)+x13*(0.371578)+x14*(0.298346)+x15*(0.390589)+x16*(0.00124055)+(-0.03283)); 
    h1 = tanh(x0*(0.0812864)+x1*(0.365265)+x2*(0.184915)+x3*(0.320245)+x4*(-0.235469)+x5*(0.0939768)+x6*(-0.0358778)+x7*(-0.327608)+x8*(0.0237349)+x9*(0.029854)+x10*(-0.397257)+x11*(0.302324)+x12*(-0.302269)+x13*(-0.347281)+x14*(0.209327)+x15*(0.113435)+x16*(-0.0706054)+(0.0584812)); 
    h2 = tanh(x0*(-0.407099)+x1*(0.076683)+x2*(0.093689)+x3*(0.289583)+x4*(-0.0933056)+x5*(-0.197813)+x6*(-0.0842324)+x7*(0.164448)+x8*(0.248325)+x9*(-0.243203)+x10*(0.353626)+x11*(0.358098)+x12*(-0.144468)+x13*(0.158684)+x14*(-0.0333445)+x15*(0.247107)+x16*(0.00225258)+(0.0211508)); 
    h3 = tanh(x0*(0.430415)+x1*(-0.0768111)+x2*(0.15398)+x3*(0.401915)+x4*(-0.27377)+x5*(0.208326)+x6*(-0.427267)+x7*(-0.166973)+x8*(-0.0946167)+x9*(0.173748)+x10*(0.138566)+x11*(-0.361019)+x12*(0.258852)+x13*(-0.251204)+x14*(0.201849)+x15*(0.270124)+x16*(0.0891326)+(-0.0172543)); 
    h4 = tanh(x0*(-0.168864)+x1*(0.324034)+x2*(0.127288)+x3*(-0.38334)+x4*(0.17398)+x5*(0.274071)+x6*(-0.57067)+x7*(0.188856)+x8*(0.328968)+x9*(-0.0475594)+x10*(0.283115)+x11*(0.265628)+x12*(-0.245427)+x13*(0.00685701)+x14*(-0.333056)+x15*(0.0567334)+x16*(-0.250725)+(-0.0584129)); 
    h5 = tanh(x0*(-0.259035)+x1*(0.0311211)+x2*(-0.338967)+x3*(-0.0151445)+x4*(0.177885)+x5*(0.234932)+x6*(0.30474)+x7*(0.00148583)+x8*(-0.0394097)+x9*(0.0913653)+x10*(0.184215)+x11*(0.0157602)+x12*(0.192839)+x13*(-0.117815)+x14*(-0.312871)+x15*(-0.059382)+x16*(0.173985)+(0.0383836)); 
    h6 = tanh(x0*(-0.399714)+x1*(-0.152994)+x2*(0.302606)+x3*(-0.180521)+x4*(0.286679)+x5*(-0.238689)+x6*(0.0295178)+x7*(0.222718)+x8*(-0.125766)+x9*(-0.297523)+x10*(-0.308981)+x11*(0.108855)+x12*(0.368804)+x13*(0.139302)+x14*(-0.321734)+x15*(-0.180016)+x16*(-0.279551)+(-0.0259766)); 
    h7 = tanh(x0*(0.0296628)+x1*(-0.104788)+x2*(-0.473284)+x3*(0.0307049)+x4*(0.332376)+x5*(-0.20262)+x6*(-0.418877)+x7*(0.244914)+x8*(-0.00392751)+x9*(-0.197573)+x10*(0.323988)+x11*(0.0653555)+x12*(0.0645303)+x13*(-0.00847247)+x14*(-0.0375714)+x15*(0.294966)+x16*(-0.132052)+(0.00235374)); 
    h8 = tanh(x0*(-0.303854)+x1*(-0.38844)+x2*(-0.00620367)+x3*(0.0301765)+x4*(-0.101232)+x5*(-0.345179)+x6*(0.177137)+x7*(0.230419)+x8*(0.352076)+x9*(-0.122323)+x10*(0.385428)+x11*(-0.24781)+x12*(0.172923)+x13*(-0.00950736)+x14*(0.0128371)+x15*(0.199151)+x16*(-0.305612)+(0.0428704)); 
    h9 = tanh(x0*(-0.0365856)+x1*(-0.402287)+x2*(0.138292)+x3*(-0.0789059)+x4*(-0.402446)+x5*(0.0648985)+x6*(0.17193)+x7*(0.00452452)+x8*(-0.30153)+x9*(-0.285882)+x10*(-0.349892)+x11*(0.220078)+x12*(-0.162039)+x13*(-0.255399)+x14*(0.280486)+x15*(-0.17467)+x16*(0.0410068)+(-0.0162232)); 
    h10 = tanh(x0*(-0.113587)+x1*(-0.0622493)+x2*(-0.0549693)+x3*(-0.209314)+x4*(0.0410513)+x5*(0.206861)+x6*(-0.207957)+x7*(0.102143)+x8*(-0.339567)+x9*(-0.112686)+x10*(-0.19383)+x11*(-0.292884)+x12*(-0.103878)+x13*(-0.357732)+x14*(-0.216759)+x15*(-0.0453324)+x16*(0.0357354)+(-0.0257021)); 
    h11 = tanh(x0*(-0.116123)+x1*(-0.224508)+x2*(-0.0446317)+x3*(0.305372)+x4*(-0.219436)+x5*(-0.0536012)+x6*(0.349084)+x7*(-0.0178004)+x8*(0.162874)+x9*(0.264022)+x10*(-0.169648)+x11*(-0.0439826)+x12*(-0.365089)+x13*(0.0629283)+x14*(-0.198264)+x15*(-0.149672)+x16*(-0.380057)+(0.0306698)); 
    h12 = tanh(x0*(0.0578949)+x1*(-0.0755422)+x2*(-0.122691)+x3*(-0.227845)+x4*(-0.344466)+x5*(-0.290948)+x6*(0.425399)+x7*(-0.0796244)+x8*(-0.375933)+x9*(-0.0996832)+x10*(-0.00209466)+x11*(-0.161224)+x12*(-0.225324)+x13*(-0.0710435)+x14*(-0.400592)+x15*(-0.357381)+x16*(0.14203)+(0.0322878)); 
    h13 = tanh(x0*(-0.0528963)+x1*(-0.0196454)+x2*(0.175837)+x3*(0.0539703)+x4*(0.28664)+x5*(-0.0640232)+x6*(-0.355769)+x7*(0.0636905)+x8*(-0.166852)+x9*(-0.109423)+x10*(-0.227313)+x11*(-0.224866)+x12*(-0.046611)+x13*(-0.0248907)+x14*(-0.0479728)+x15*(-0.15034)+x16*(-0.20223)+(-0.0168174)); 
    h14 = tanh(x0*(0.192879)+x1*(-0.152959)+x2*(-0.337426)+x3*(-0.160495)+x4*(-0.0170364)+x5*(-0.28348)+x6*(-0.236344)+x7*(0.358322)+x8*(-0.10975)+x9*(-0.302326)+x10*(-0.257306)+x11*(-0.343999)+x12*(0.284427)+x13*(0.0728938)+x14*(0.0234528)+x15*(0.333144)+x16*(0.175571)+(0.0330101)); 
    h15 = tanh(x0*(0.241224)+x1*(0.294039)+x2*(0.275424)+x3*(0.0600129)+x4*(-0.235821)+x5*(0.0929524)+x6*(0.0696455)+x7*(0.0541981)+x8*(0.263136)+x9*(-0.0337297)+x10*(-0.264591)+x11*(0.241099)+x12*(0.186735)+x13*(-0.0145856)+x14*(0.257075)+x15*(0.0818634)+x16*(0.285011)+(-0.0366464)); 
    h16 = tanh(x0*(-0.246144)+x1*(-0.185004)+x2*(0.142641)+x3*(0.31961)+x4*(-0.337956)+x5*(-0.043385)+x6*(0.356817)+x7*(-0.0693889)+x8*(0.278891)+x9*(0.165211)+x10*(-0.317303)+x11*(0.259404)+x12*(-0.119245)+x13*(0.0325929)+x14*(-0.322136)+x15*(0.253496)+x16*(0.164061)+(-0.0351229)); 
    h17 = tanh(x0*(-0.371684)+x1*(0.35194)+x2*(-0.271212)+x3*(-0.0749729)+x4*(-0.164662)+x5*(-0.474569)+x6*(0.0725434)+x7*(0.0871262)+x8*(-0.0151545)+x9*(0.325515)+x10*(-0.133086)+x11*(-0.225343)+x12*(0.143645)+x13*(-0.392353)+x14*(0.203179)+x15*(-0.0387519)+x16*(-0.365911)+(-0.0507877)); 
    h18 = tanh(x0*(0.234301)+x1*(-0.459395)+x2*(-0.369431)+x3*(0.193338)+x4*(-0.212705)+x5*(-0.246929)+x6*(0.00904283)+x7*(-0.0415067)+x8*(0.137475)+x9*(0.255924)+x10*(-0.304863)+x11*(-0.28356)+x12*(0.366858)+x13*(-0.0822717)+x14*(-0.193838)+x15*(0.180879)+x16*(-0.337167)+(0.0306832)); 
    h19 = tanh(x0*(0.0192285)+x1*(0.305597)+x2*(-0.214179)+x3*(-0.342159)+x4*(-0.226513)+x5*(-0.303911)+x6*(0.302511)+x7*(0.123691)+x8*(-0.0100141)+x9*(0.195857)+x10*(-0.217881)+x11*(0.323467)+x12*(-0.217874)+x13*(-0.355333)+x14*(0.226601)+x15*(-0.13733)+x16*(0.0391881)+(0.0179068)); 
}

void FNN_16_1_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(0.109358)+x1*(-0.178193)+x2*(-0.320983)+x3*(0.393971)+x4*(0.269346)+x5*(-0.230028)+x6*(0.184292)+x7*(-0.449441)+x8*(-0.418532)+x9*(0.181711)+x10*(0.0990467)+x11*(-0.071607)+x12*(0.0402287)+x13*(0.354513)+x14*(0.165025)+x15*(-0.297597)+x16*(0.35958)+x17*(0.266449)+x18*(-0.1243)+x19*(-0.364902)+(-0.0441209)); 
    h1 = tanh(x0*(0.162675)+x1*(-0.389878)+x2*(0.212816)+x3*(-0.398249)+x4*(-0.406013)+x5*(-0.103673)+x6*(-0.0737215)+x7*(0.33994)+x8*(0.417913)+x9*(0.404571)+x10*(-0.0679069)+x11*(-0.264299)+x12*(-0.029404)+x13*(-0.0573055)+x14*(0.254827)+x15*(0.383032)+x16*(0.132891)+x17*(-0.132831)+x18*(0.370977)+x19*(0.279569)+(-0.0342664)); 
    h2 = tanh(x0*(-0.380438)+x1*(0.326616)+x2*(0.376573)+x3*(0.149422)+x4*(-0.0357041)+x5*(0.366453)+x6*(-0.125632)+x7*(0.41333)+x8*(-0.309998)+x9*(-0.384456)+x10*(-0.0748006)+x11*(0.369351)+x12*(0.236033)+x13*(-0.253755)+x14*(-0.261311)+x15*(0.176887)+x16*(-0.0762198)+x17*(-0.190769)+x18*(0.302109)+x19*(0.39294)+(0.0219363)); 
    h3 = tanh(x0*(-0.00017098)+x1*(0.0962221)+x2*(0.0567357)+x3*(-0.0222351)+x4*(0.1447)+x5*(0.0679117)+x6*(-0.265387)+x7*(0.0525164)+x8*(-0.274021)+x9*(-0.169597)+x10*(-0.114547)+x11*(-0.244754)+x12*(0.366977)+x13*(0.203401)+x14*(-0.483929)+x15*(0.171773)+x16*(-0.361216)+x17*(-0.0411419)+x18*(0.250612)+x19*(0.206182)+(-0.0359997)); 
    h4 = tanh(x0*(0.171312)+x1*(-0.522928)+x2*(0.404444)+x3*(0.258842)+x4*(-0.223534)+x5*(0.0413)+x6*(0.0332897)+x7*(-0.183172)+x8*(-0.35224)+x9*(0.0815188)+x10*(-0.329351)+x11*(0.306357)+x12*(0.303736)+x13*(-0.294173)+x14*(0.355799)+x15*(0.0455562)+x16*(-0.244321)+x17*(-0.437542)+x18*(0.259804)+x19*(-0.33705)+(0.01292)); 
    h5 = tanh(x0*(0.209854)+x1*(0.192316)+x2*(-0.263432)+x3*(0.167621)+x4*(-0.0732683)+x5*(0.355255)+x6*(0.298734)+x7*(0.0697181)+x8*(0.0624208)+x9*(0.423423)+x10*(0.134575)+x11*(0.370199)+x12*(-0.00491928)+x13*(0.407451)+x14*(0.12845)+x15*(-0.373209)+x16*(0.0674006)+x17*(-0.271814)+x18*(-0.0975715)+x19*(0.12561)+(0.03167)); 
    h6 = tanh(x0*(-0.0273223)+x1*(0.0169347)+x2*(0.0047978)+x3*(-0.3455)+x4*(0.318827)+x5*(-0.234211)+x6*(0.22684)+x7*(-0.173626)+x8*(-0.212304)+x9*(-0.186242)+x10*(-0.331515)+x11*(-0.165932)+x12*(0.345929)+x13*(-0.394649)+x14*(0.201876)+x15*(-0.185705)+x16*(-0.249759)+x17*(-0.00989807)+x18*(0.397624)+x19*(-0.237234)+(0.0246746)); 
    h7 = tanh(x0*(-0.368819)+x1*(0.0951748)+x2*(0.442118)+x3*(-0.175112)+x4*(-0.181713)+x5*(0.344687)+x6*(0.403644)+x7*(-0.0426895)+x8*(0.209909)+x9*(0.281134)+x10*(-0.0440651)+x11*(0.272023)+x12*(-0.0135604)+x13*(0.0942561)+x14*(0.227201)+x15*(0.321905)+x16*(-0.304997)+x17*(0.153873)+x18*(0.421186)+x19*(0.182603)+(-0.0266964)); 
    h8 = tanh(x0*(0.224264)+x1*(0.368593)+x2*(-0.0124913)+x3*(-0.00265669)+x4*(-0.0255977)+x5*(-0.168885)+x6*(-0.0435655)+x7*(-0.121517)+x8*(-0.0986781)+x9*(-0.260918)+x10*(0.116086)+x11*(-0.273762)+x12*(-0.332377)+x13*(0.181805)+x14*(-0.22873)+x15*(-0.212587)+x16*(-0.392667)+x17*(0.201518)+x18*(0.047835)+x19*(0.408618)+(-0.0254085)); 
    h9 = tanh(x0*(0.0794468)+x1*(0.220093)+x2*(-0.102679)+x3*(0.197158)+x4*(0.224397)+x5*(0.28069)+x6*(-0.469512)+x7*(-0.151835)+x8*(-0.457747)+x9*(0.18335)+x10*(0.0146781)+x11*(-0.235851)+x12*(-0.215225)+x13*(0.120081)+x14*(-0.0644911)+x15*(-0.107049)+x16*(-0.189199)+x17*(0.154614)+x18*(-0.304313)+x19*(0.0696437)+(0.0270239)); 
}

void FNN_16_1_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(-0.460423)+x1*(-0.150997)+x2*(0.0215498)+x3*(0.0126782)+x4*(-0.31551)+x5*(0.102694)+x6*(-0.534812)+x7*(0.460626)+x8*(-0.212174)+x9*(0.49552)+(-0.00854622); 
    h1 = x0*(-0.279664)+x1*(-0.332424)+x2*(-0.00926112)+x3*(-0.179375)+x4*(0.31473)+x5*(0.12892)+x6*(-0.329591)+x7*(0.0543236)+x8*(0.148213)+x9*(-0.161888)+(0.00963321); 
    h2 = x0*(-0.588616)+x1*(0.0979352)+x2*(0.0254743)+x3*(-0.0601204)+x4*(0.244669)+x5*(0.354549)+x6*(0.473318)+x7*(-0.334969)+x8*(-0.518011)+x9*(0.586574)+(0.0274864); 
}

void FNN_16_1(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_16_1_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_16_1_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_16_1_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_1(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_16_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void FNN_16_2_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(-0.0219124)+x1*(0.0664942)+x2*(0.00593102)+x3*(-0.0830987)+x4*(0.1328)+x5*(0.107719)+x6*(0.224284)+x7*(-0.149035)+x8*(-0.328702)+x9*(-0.0796811)+x10*(0.153427)+x11*(0.0587605)+x12*(-0.348945)+x13*(-0.262914)+x14*(0.202887)+x15*(0.247948)+x16*(-0.396715)+(-0.0264267)); 
    h1 = tanh(x0*(0.301805)+x1*(0.303172)+x2*(0.274026)+x3*(-0.179989)+x4*(-0.368069)+x5*(0.151191)+x6*(-0.434357)+x7*(0.188661)+x8*(-0.283968)+x9*(-0.382382)+x10*(0.239369)+x11*(-0.019403)+x12*(0.266955)+x13*(0.0786536)+x14*(0.0335942)+x15*(-0.343436)+x16*(-0.0278698)+(-0.0418198)); 
    h2 = tanh(x0*(-0.353838)+x1*(0.00982587)+x2*(-0.058407)+x3*(0.180696)+x4*(0.203903)+x5*(0.329744)+x6*(-0.225358)+x7*(-0.242378)+x8*(-0.29423)+x9*(0.185174)+x10*(0.158982)+x11*(-0.323598)+x12*(0.142073)+x13*(-0.395218)+x14*(0.167059)+x15*(-0.185061)+x16*(-0.340156)+(-0.0433314)); 
    h3 = tanh(x0*(-0.272839)+x1*(0.416752)+x2*(0.331566)+x3*(-0.285206)+x4*(0.400411)+x5*(0.191767)+x6*(0.662525)+x7*(-0.213117)+x8*(-0.0329963)+x9*(-0.298013)+x10*(-0.28928)+x11*(0.261573)+x12*(0.23334)+x13*(0.37983)+x14*(0.233447)+x15*(-0.371358)+x16*(0.400642)+(0.0303551)); 
    h4 = tanh(x0*(-0.263332)+x1*(0.133223)+x2*(-0.367854)+x3*(-0.181083)+x4*(0.368147)+x5*(0.085758)+x6*(0.308763)+x7*(0.296416)+x8*(0.107145)+x9*(-0.162161)+x10*(-0.0881361)+x11*(-0.000579894)+x12*(-0.182354)+x13*(-0.0793957)+x14*(-0.274144)+x15*(0.359962)+x16*(0.197554)+(-0.004278)); 
    h5 = tanh(x0*(0.0186178)+x1*(0.444821)+x2*(0.259835)+x3*(-0.0452419)+x4*(-0.400298)+x5*(0.0224893)+x6*(-0.12946)+x7*(-0.160302)+x8*(0.0338054)+x9*(0.0695506)+x10*(-0.379503)+x11*(-0.00378036)+x12*(0.295895)+x13*(-0.163678)+x14*(0.114066)+x15*(0.164731)+x16*(-0.109349)+(-0.000105191)); 
    h6 = tanh(x0*(-0.346471)+x1*(-0.244844)+x2*(0.175024)+x3*(0.348579)+x4*(0.0751307)+x5*(-0.246658)+x6*(0.402005)+x7*(0.270907)+x8*(0.0826458)+x9*(0.0114151)+x10*(-0.157986)+x11*(0.371804)+x12*(-0.061682)+x13*(0.231297)+x14*(0.0963042)+x15*(-0.380718)+x16*(0.141189)+(0.0075821)); 
    h7 = tanh(x0*(0.191434)+x1*(0.191266)+x2*(0.0178682)+x3*(0.0840567)+x4*(-0.119856)+x5*(0.419989)+x6*(0.0853387)+x7*(-0.0601)+x8*(0.13145)+x9*(0.135892)+x10*(0.0983348)+x11*(0.195741)+x12*(0.209918)+x13*(-0.334965)+x14*(-0.139953)+x15*(0.316221)+x16*(0.31778)+(0.00671948)); 
    h8 = tanh(x0*(-0.207052)+x1*(0.331783)+x2*(0.362766)+x3*(-0.260641)+x4*(0.197983)+x5*(0.14497)+x6*(0.097077)+x7*(-0.0361431)+x8*(0.325324)+x9*(-0.00323007)+x10*(0.394667)+x11*(-0.366417)+x12*(0.355948)+x13*(0.288459)+x14*(0.301441)+x15*(-0.380704)+x16*(0.0883891)+(0.000710446)); 
    h9 = tanh(x0*(0.0890488)+x1*(-0.233946)+x2*(0.0806772)+x3*(0.23409)+x4*(0.189766)+x5*(0.00981723)+x6*(0.130016)+x7*(-0.0688171)+x8*(0.196317)+x9*(0.058854)+x10*(-0.333244)+x11*(0.0923048)+x12*(0.0435095)+x13*(0.233666)+x14*(0.0302595)+x15*(0.267802)+x16*(0.273384)+(-0.0442617)); 
    h10 = tanh(x0*(0.0476825)+x1*(-0.223666)+x2*(-0.368691)+x3*(-0.0994741)+x4*(-0.203123)+x5*(0.169649)+x6*(-0.282347)+x7*(-0.126869)+x8*(-0.306465)+x9*(0.394929)+x10*(-0.0179747)+x11*(0.284275)+x12*(0.369475)+x13*(0.135733)+x14*(0.1216)+x15*(0.381088)+x16*(0.180856)+(0.0182938)); 
    h11 = tanh(x0*(0.227618)+x1*(0.0852364)+x2*(0.163765)+x3*(0.352795)+x4*(-0.0728874)+x5*(0.251745)+x6*(0.495731)+x7*(0.0144262)+x8*(0.220919)+x9*(-0.0457888)+x10*(-0.337508)+x11*(0.276526)+x12*(-0.0371625)+x13*(0.150808)+x14*(-0.206281)+x15*(-0.301339)+x16*(-0.0657115)+(-0.0100387)); 
    h12 = tanh(x0*(-0.222343)+x1*(0.197789)+x2*(0.21989)+x3*(-0.18886)+x4*(0.166245)+x5*(-0.0844951)+x6*(-0.0310318)+x7*(0.113677)+x8*(-0.393906)+x9*(0.0257241)+x10*(0.277651)+x11*(0.2055)+x12*(0.342198)+x13*(0.314283)+x14*(0.394736)+x15*(-0.257979)+x16*(0.352289)+(-0.0142365)); 
    h13 = tanh(x0*(-0.1507)+x1*(-0.302655)+x2*(-0.333136)+x3*(0.342066)+x4*(-0.352071)+x5*(0.26811)+x6*(-0.119565)+x7*(-0.307348)+x8*(-0.123454)+x9*(0.28931)+x10*(-0.380585)+x11*(-0.236052)+x12*(-0.375635)+x13*(0.281093)+x14*(0.0578712)+x15*(-0.0380318)+x16*(0.170611)+(-0.0123235)); 
    h14 = tanh(x0*(0.106832)+x1*(-0.254805)+x2*(0.0307394)+x3*(0.148909)+x4*(0.0871)+x5*(-0.346942)+x6*(0.26018)+x7*(-0.0806423)+x8*(-0.133914)+x9*(0.218917)+x10*(0.0882282)+x11*(-0.0816889)+x12*(0.365696)+x13*(0.257194)+x14*(-0.250946)+x15*(-0.192283)+x16*(-0.195182)+(0.0151299)); 
    h15 = tanh(x0*(-0.330712)+x1*(0.424472)+x2*(0.303132)+x3*(-0.0559601)+x4*(0.0744092)+x5*(-0.0669172)+x6*(-0.161818)+x7*(-0.300478)+x8*(0.12434)+x9*(0.271688)+x10*(-0.255781)+x11*(-0.232205)+x12*(-0.335712)+x13*(-0.38397)+x14*(-0.135456)+x15*(-0.173381)+x16*(0.318566)+(0.0194163)); 
    h16 = tanh(x0*(0.191025)+x1*(0.309075)+x2*(0.0301604)+x3*(-0.0236955)+x4*(0.287448)+x5*(-0.0403364)+x6*(-0.54498)+x7*(-0.216769)+x8*(-0.243498)+x9*(-0.298949)+x10*(0.0625581)+x11*(0.240144)+x12*(-0.24608)+x13*(-0.309318)+x14*(0.0557491)+x15*(-0.381744)+x16*(-0.0338733)+(-0.000988169)); 
    h17 = tanh(x0*(0.1238)+x1*(0.142645)+x2*(-0.00870369)+x3*(0.0943292)+x4*(0.0430147)+x5*(0.425457)+x6*(-0.227531)+x7*(0.252417)+x8*(-0.303535)+x9*(-0.378844)+x10*(0.285762)+x11*(-0.231516)+x12*(-0.385299)+x13*(0.136984)+x14*(0.173839)+x15*(0.334879)+x16*(-0.0179696)+(0.0140915)); 
    h18 = tanh(x0*(0.261005)+x1*(0.0430495)+x2*(-0.229303)+x3*(0.345258)+x4*(-0.364175)+x5*(-0.176215)+x6*(0.147808)+x7*(-0.316605)+x8*(0.223485)+x9*(-0.219868)+x10*(-0.122298)+x11*(-0.0596203)+x12*(0.361534)+x13*(0.309428)+x14*(-0.0288787)+x15*(0.196957)+x16*(0.276836)+(-0.000585777)); 
    h19 = tanh(x0*(-0.207945)+x1*(0.288056)+x2*(-0.0690667)+x3*(-0.319037)+x4*(-0.285673)+x5*(-0.208718)+x6*(-0.215633)+x7*(0.254399)+x8*(-0.142107)+x9*(-0.224008)+x10*(0.0242636)+x11*(0.33692)+x12*(-0.29301)+x13*(0.299891)+x14*(-0.307544)+x15*(0.148907)+x16*(-0.147846)+(-0.00649827)); 
}

void FNN_16_2_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(-0.0604075)+x1*(-0.157036)+x2*(0.288394)+x3*(0.0938561)+x4*(0.0541813)+x5*(0.441567)+x6*(-0.0711433)+x7*(0.338873)+x8*(-0.192817)+x9*(-0.0895065)+x10*(-0.123067)+x11*(0.236775)+x12*(0.313795)+x13*(0.233543)+x14*(-0.111765)+x15*(0.30513)+x16*(-0.30975)+x17*(-0.429116)+x18*(-0.187659)+x19*(0.0136397)+(-0.030932)); 
    h1 = tanh(x0*(-0.00563255)+x1*(-0.52308)+x2*(0.152601)+x3*(-0.327636)+x4*(-0.0751047)+x5*(0.0564226)+x6*(-0.0450263)+x7*(0.249628)+x8*(-0.0351607)+x9*(0.0232438)+x10*(-0.258465)+x11*(-0.0145546)+x12*(-0.181431)+x13*(-0.197762)+x14*(0.437386)+x15*(-0.138741)+x16*(-0.154728)+x17*(0.0679938)+x18*(0.0311008)+x19*(0.203148)+(-0.0151045)); 
    h2 = tanh(x0*(-0.267902)+x1*(-0.181879)+x2*(0.130688)+x3*(-0.0852244)+x4*(-0.386003)+x5*(-0.279171)+x6*(0.0144231)+x7*(0.175764)+x8*(0.154828)+x9*(0.344213)+x10*(-0.229157)+x11*(0.164783)+x12*(-0.283796)+x13*(-0.145028)+x14*(-0.0816768)+x15*(-0.0365423)+x16*(-0.224952)+x17*(0.48932)+x18*(0.303432)+x19*(0.14456)+(-0.0150109)); 
    h3 = tanh(x0*(0.44826)+x1*(0.496931)+x2*(0.207084)+x3*(-0.167106)+x4*(-0.0368309)+x5*(0.246393)+x6*(0.0751408)+x7*(-0.398287)+x8*(-0.121048)+x9*(0.229445)+x10*(-0.159183)+x11*(0.236232)+x12*(0.227229)+x13*(0.201302)+x14*(0.206914)+x15*(0.0740419)+x16*(0.361692)+x17*(-0.123565)+x18*(-0.469163)+x19*(-0.11259)+(-0.0197224)); 
    h4 = tanh(x0*(-0.132539)+x1*(0.0479152)+x2*(-0.340257)+x3*(0.223063)+x4*(-0.0897045)+x5*(0.229244)+x6*(-0.102224)+x7*(0.271139)+x8*(-0.269206)+x9*(-0.139681)+x10*(-0.198641)+x11*(0.263945)+x12*(0.117397)+x13*(0.00567673)+x14*(-0.205829)+x15*(-0.00905187)+x16*(-0.0648399)+x17*(-0.394553)+x18*(-0.135984)+x19*(-0.101039)+(0.0428451)); 
    h5 = tanh(x0*(-0.372964)+x1*(0.0809423)+x2*(-0.121463)+x3*(-0.0135367)+x4*(0.318936)+x5*(-0.228124)+x6*(-0.223712)+x7*(-0.423439)+x8*(-0.115697)+x9*(-0.161063)+x10*(-0.323543)+x11*(-0.00303705)+x12*(-0.0171534)+x13*(-0.0387192)+x14*(-0.153488)+x15*(-0.0424158)+x16*(-0.321833)+x17*(-0.333515)+x18*(0.353192)+x19*(0.00137904)+(-0.0103998)); 
    h6 = tanh(x0*(-0.102461)+x1*(0.356161)+x2*(0.253617)+x3*(0.0287985)+x4*(-0.25056)+x5*(-0.130734)+x6*(0.10717)+x7*(-0.0902156)+x8*(0.265111)+x9*(0.0400957)+x10*(0.0323337)+x11*(0.322725)+x12*(-0.130266)+x13*(-0.0759307)+x14*(-0.142051)+x15*(-0.212132)+x16*(-0.181244)+x17*(0.0898131)+x18*(0.271396)+x19*(-0.269584)+(-0.0400936)); 
    h7 = tanh(x0*(0.290499)+x1*(0.33296)+x2*(0.0819019)+x3*(0.0145434)+x4*(0.0445397)+x5*(-0.132434)+x6*(0.426807)+x7*(0.182136)+x8*(0.303643)+x9*(0.356374)+x10*(0.136941)+x11*(-0.403376)+x12*(-0.0977649)+x13*(-0.135471)+x14*(0.145029)+x15*(-0.0653807)+x16*(-0.458409)+x17*(0.258221)+x18*(-0.179357)+x19*(0.256768)+(-0.00123293)); 
    h8 = tanh(x0*(-0.0936626)+x1*(0.0757921)+x2*(0.123314)+x3*(0.343954)+x4*(-0.510503)+x5*(0.0800895)+x6*(-0.121522)+x7*(-0.166172)+x8*(0.419881)+x9*(0.188464)+x10*(-0.0764327)+x11*(-0.380562)+x12*(-0.0787177)+x13*(-0.463469)+x14*(0.333131)+x15*(-0.272816)+x16*(0.12945)+x17*(0.148586)+x18*(0.334904)+x19*(-0.165569)+(-0.0118038)); 
    h9 = tanh(x0*(-0.144857)+x1*(0.238445)+x2*(0.132714)+x3*(0.21665)+x4*(0.301909)+x5*(0.430002)+x6*(-0.0370512)+x7*(-0.4117)+x8*(-0.0959562)+x9*(0.362235)+x10*(0.0905208)+x11*(0.127168)+x12*(-0.0121834)+x13*(0.273911)+x14*(-0.197939)+x15*(0.2418)+x16*(-0.112049)+x17*(0.250989)+x18*(-0.448959)+x19*(0.339783)+(-0.00903522)); 
}

void FNN_16_2_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(-0.0920357)+x1*(-0.460794)+x2*(0.0802873)+x3*(-0.341431)+x4*(0.210764)+x5*(-0.477483)+x6*(-0.446162)+x7*(0.158268)+x8*(-0.347445)+x9*(-0.632724)+(0.0118145); 
    h1 = x0*(0.325845)+x1*(-0.392987)+x2*(0.579056)+x3*(0.0759419)+x4*(0.0352344)+x5*(-0.0932352)+x6*(-0.594022)+x7*(0.270237)+x8*(-0.225546)+x9*(-0.439859)+(-0.0232237); 
    h2 = x0*(0.260026)+x1*(0.70717)+x2*(-0.342177)+x3*(-0.338199)+x4*(-0.268092)+x5*(-0.5569)+x6*(0.541748)+x7*(0.37997)+x8*(0.000706496)+x9*(0.00670306)+(-0.00851435); 
}

void FNN_16_2(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_16_2_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_16_2_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_16_2_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_2(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_16_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void ModelKDTree_0(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    float interpolateExtent = 1* interpolateRatio;
    float delta = interpolateExtent / 2;
    float lowBound = 0.5 - delta;
    float highBound = 0.5 + delta;
    if ( x3 < lowBound ) {
        ModelKDTree_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
    }
    else if ( x3 < highBound ) {
        float left_h0;
        float left_h1;
        float left_h2;
        float right_h0;
        float right_h1;
        float right_h2;
        
        ModelKDTree_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,left_h0,left_h1,left_h2);
        ModelKDTree_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,right_h0,right_h1,right_h2);
        
        float t = 0.5 + ( x3 - 0.5 ) / interpolateExtent;
        t = smootherstep(t);
        h0= mix(left_h0,right_h0, t);
        h1= mix(left_h1,right_h1, t);
        h2= mix(left_h2,right_h2, t);
    }
    else {
        ModelKDTree_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
    }
}

void Model(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,    
out float h0,out float h1,out float h2)
{
    // map to unit hyperbox
    x0 = (x0 - (0.024887)) / (0.167984);
    x1 = (x1 - (0.024887)) / (0.167984);
    x2 = (x2 - (0.074646)) / (0.503967);
    x3 = (x3 - (0.999512)) / (0.000488);
    x4 = (x4 - (7.2e-05)) / (1.49895);
    x5 = (x5 - (-0.999512)) / (1.99902);
    x6 = (x6 - (-0.999512)) / (0.868775);
    x7 = (x7 - (-0.96582)) / (1.92871);
    x8 = (x8 - (-0.932617)) / (1.78955);
    x9 = (x9 - (-1)) / (1);
    x10 = (x10 - (0)) / (1);
    x11 = (x11 - (0)) / (1);
    x12 = (x12 - (0.199951)) / (1);
    x13 = (x13 - (0.199951)) / (1);
    x14 = (x14 - (0.599609)) / (1);
    x15 = (x15 - (20)) / (1);
    x16 = (x16 - (1)) / (1);

    ModelKDTree_0(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);

    // map back
    h0 = h0 * (0.31191) + (0);
    h1 = h1 * (0.315832) + (0);
    h2 = h2 * (0.320106) + (0);
}

