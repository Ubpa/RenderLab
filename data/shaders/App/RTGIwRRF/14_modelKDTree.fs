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

void FNN_14_1_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(0.0490589)+x1*(-0.318703)+x2*(-0.21588)+x3*(-0.151379)+x4*(-0.504868)+x5*(-0.110209)+x6*(-0.118898)+x7*(-0.203625)+x8*(-0.025253)+x9*(0.176591)+x10*(0.280296)+x11*(0.0364982)+x12*(0.0845039)+x13*(0.16414)+x14*(0.370824)+x15*(0.293279)+x16*(0.37882)+(0.373234)); 
    h1 = tanh(x0*(0.215172)+x1*(-0.261807)+x2*(0.3052)+x3*(-0.105727)+x4*(0.116226)+x5*(-0.89527)+x6*(-0.289546)+x7*(-0.32839)+x8*(-0.0196044)+x9*(0.349188)+x10*(-0.0378569)+x11*(-0.100445)+x12*(0.0331872)+x13*(0.150281)+x14*(0.131233)+x15*(-0.277282)+x16*(0.220607)+(-0.188919)); 
    h2 = tanh(x0*(0.384046)+x1*(-0.0428232)+x2*(0.406515)+x3*(0.048586)+x4*(-0.137599)+x5*(-0.054442)+x6*(-0.488562)+x7*(0.0642362)+x8*(-0.386404)+x9*(-0.0737164)+x10*(-0.389683)+x11*(0.0436558)+x12*(-0.106622)+x13*(0.395875)+x14*(-0.285874)+x15*(-0.210797)+x16*(0.102316)+(-0.260572)); 
    h3 = tanh(x0*(0.0591652)+x1*(-0.052719)+x2*(0.221123)+x3*(0.0736722)+x4*(0.0227829)+x5*(0.256286)+x6*(0.0346389)+x7*(-0.393695)+x8*(0.373253)+x9*(-0.140226)+x10*(-0.262311)+x11*(-0.3306)+x12*(-0.231927)+x13*(0.361522)+x14*(-0.178147)+x15*(0.199843)+x16*(0.342029)+(-0.0964662)); 
    h4 = tanh(x0*(-0.25576)+x1*(-0.256509)+x2*(-0.0236619)+x3*(-0.151785)+x4*(0.0916585)+x5*(-0.783155)+x6*(0.360161)+x7*(0.153827)+x8*(0.0701089)+x9*(-0.119128)+x10*(0.356626)+x11*(-0.245883)+x12*(0.0876455)+x13*(-0.0262015)+x14*(-0.365655)+x15*(-0.372682)+x16*(-0.0246899)+(0.129224)); 
    h5 = tanh(x0*(-0.136222)+x1*(0.169242)+x2*(0.353884)+x3*(-0.328537)+x4*(-0.26225)+x5*(0.436446)+x6*(0.0525543)+x7*(0.0203631)+x8*(0.234779)+x9*(0.337841)+x10*(-0.112822)+x11*(0.190709)+x12*(-0.164702)+x13*(-0.312823)+x14*(0.198168)+x15*(-0.22421)+x16*(-0.0569256)+(-0.24915)); 
    h6 = tanh(x0*(0.25952)+x1*(-0.216232)+x2*(0.0550764)+x3*(-0.208907)+x4*(-0.231371)+x5*(-0.0328078)+x6*(-0.26617)+x7*(-0.137086)+x8*(-0.0654025)+x9*(-0.12065)+x10*(-0.117522)+x11*(0.232975)+x12*(-0.0813907)+x13*(-0.217194)+x14*(0.118893)+x15*(0.170845)+x16*(-0.110909)+(0.0525782)); 
    h7 = tanh(x0*(-0.164777)+x1*(0.0483372)+x2*(-0.00937739)+x3*(0.158102)+x4*(-0.148268)+x5*(-0.222006)+x6*(-0.161171)+x7*(0.497752)+x8*(0.152098)+x9*(-0.0208968)+x10*(0.227198)+x11*(-0.0899181)+x12*(0.0509075)+x13*(-0.133746)+x14*(-0.354417)+x15*(-0.306484)+x16*(-0.289308)+(-0.0156483)); 
    h8 = tanh(x0*(0.364114)+x1*(0.0858848)+x2*(-0.163497)+x3*(0.138989)+x4*(-0.25047)+x5*(-0.0560136)+x6*(0.234671)+x7*(0.471708)+x8*(0.0214125)+x9*(0.137955)+x10*(-0.254162)+x11*(0.310403)+x12*(0.129326)+x13*(-0.263581)+x14*(0.0848876)+x15*(-0.268541)+x16*(-0.397375)+(-0.186204)); 
    h9 = tanh(x0*(0.128066)+x1*(-0.0331252)+x2*(0.191171)+x3*(0.382317)+x4*(0.0834635)+x5*(0.483957)+x6*(-0.207877)+x7*(0.13302)+x8*(0.13172)+x9*(0.361962)+x10*(-0.0695948)+x11*(0.205174)+x12*(0.398992)+x13*(-0.0282937)+x14*(0.355235)+x15*(-0.292619)+x16*(0.269286)+(-0.365016)); 
    h10 = tanh(x0*(0.298397)+x1*(0.297497)+x2*(0.102347)+x3*(-0.491835)+x4*(0.236509)+x5*(-0.643195)+x6*(-0.224392)+x7*(0.448058)+x8*(0.160584)+x9*(-0.134161)+x10*(-0.340017)+x11*(0.0538275)+x12*(-0.401075)+x13*(0.21687)+x14*(-0.324787)+x15*(-0.105038)+x16*(0.334613)+(0.265127)); 
    h11 = tanh(x0*(-0.236907)+x1*(0.169321)+x2*(0.234848)+x3*(0.17186)+x4*(-0.0492807)+x5*(-0.656043)+x6*(0.393264)+x7*(-0.0292102)+x8*(0.233494)+x9*(-0.237638)+x10*(-0.181649)+x11*(0.119322)+x12*(0.304699)+x13*(0.182912)+x14*(-0.11854)+x15*(0.318144)+x16*(-0.0153886)+(0.255556)); 
    h12 = tanh(x0*(-0.310395)+x1*(0.0213216)+x2*(0.427626)+x3*(-0.316623)+x4*(0.187717)+x5*(0.231857)+x6*(0.381874)+x7*(0.289932)+x8*(-0.204318)+x9*(-0.0984002)+x10*(-0.258769)+x11*(0.324762)+x12*(-0.34466)+x13*(0.376198)+x14*(-0.362749)+x15*(-0.219301)+x16*(0.294944)+(-0.0856485)); 
    h13 = tanh(x0*(-0.21966)+x1*(-0.14918)+x2*(-0.0536882)+x3*(0.450416)+x4*(-0.212089)+x5*(0.357568)+x6*(-0.0822917)+x7*(-0.15883)+x8*(-0.25786)+x9*(0.245851)+x10*(0.125226)+x11*(-0.0246881)+x12*(-0.202732)+x13*(-0.0242669)+x14*(-0.257897)+x15*(0.116545)+x16*(-0.142544)+(-0.0268373)); 
    h14 = tanh(x0*(-0.0201838)+x1*(0.28679)+x2*(-0.243319)+x3*(0.571489)+x4*(-0.523932)+x5*(-0.116441)+x6*(0.206299)+x7*(-0.09549)+x8*(0.0680351)+x9*(0.0140106)+x10*(0.362193)+x11*(0.121655)+x12*(0.0400903)+x13*(-0.299296)+x14*(0.145755)+x15*(0.0542166)+x16*(0.373439)+(0.245329)); 
    h15 = tanh(x0*(0.175334)+x1*(0.0912032)+x2*(0.0420065)+x3*(0.209606)+x4*(-0.104675)+x5*(0.171228)+x6*(0.259626)+x7*(0.0200566)+x8*(-0.161551)+x9*(-0.00636765)+x10*(0.0897723)+x11*(0.19227)+x12*(-0.00593513)+x13*(-0.321633)+x14*(-0.260471)+x15*(0.0931798)+x16*(-0.00118169)+(-0.218596)); 
    h16 = tanh(x0*(0.312711)+x1*(-0.34756)+x2*(0.0877141)+x3*(0.176894)+x4*(0.178221)+x5*(0.109975)+x6*(-0.208266)+x7*(0.216687)+x8*(-0.321185)+x9*(0.105949)+x10*(-0.0769182)+x11*(-0.2968)+x12*(-0.339797)+x13*(-0.194632)+x14*(-0.052768)+x15*(0.29791)+x16*(-0.222006)+(-0.0191925)); 
    h17 = tanh(x0*(0.270345)+x1*(0.139518)+x2*(-0.267214)+x3*(-0.202358)+x4*(0.260231)+x5*(0.314089)+x6*(0.0247621)+x7*(0.22419)+x8*(-0.135749)+x9*(-0.323698)+x10*(-0.0188095)+x11*(-0.101613)+x12*(0.0407071)+x13*(0.018362)+x14*(-0.139619)+x15*(-0.0349041)+x16*(0.295694)+(-0.168918)); 
    h18 = tanh(x0*(0.304883)+x1*(-0.070539)+x2*(0.0495042)+x3*(0.0131048)+x4*(0.254812)+x5*(0.266073)+x6*(-0.0152945)+x7*(0.435144)+x8*(0.100959)+x9*(-0.173158)+x10*(0.244722)+x11*(0.0222177)+x12*(0.293851)+x13*(0.256456)+x14*(0.12171)+x15*(-0.0753774)+x16*(0.250937)+(-0.297961)); 
    h19 = tanh(x0*(-0.306114)+x1*(-0.453473)+x2*(-0.434545)+x3*(0.161754)+x4*(0.0225205)+x5*(-0.0600196)+x6*(0.0357343)+x7*(0.144402)+x8*(0.181195)+x9*(0.156717)+x10*(0.32196)+x11*(0.137056)+x12*(-0.333849)+x13*(0.174298)+x14*(-0.318311)+x15*(-0.0420728)+x16*(-0.286281)+(0.0789083)); 
}

void FNN_14_1_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(0.248203)+x1*(-0.453029)+x2*(-0.271743)+x3*(-0.363881)+x4*(-0.00940735)+x5*(-0.379751)+x6*(0.246147)+x7*(-0.312364)+x8*(-0.341248)+x9*(-0.114733)+x10*(0.0254208)+x11*(0.267584)+x12*(-0.216266)+x13*(-0.095813)+x14*(0.0947227)+x15*(-0.382504)+x16*(-0.00474959)+x17*(-0.403063)+x18*(-0.326268)+x19*(-0.149527)+(0.117356)); 
    h1 = tanh(x0*(0.198866)+x1*(0.0488792)+x2*(-0.00783639)+x3*(0.254915)+x4*(-0.107577)+x5*(-0.154738)+x6*(-0.425273)+x7*(-0.323192)+x8*(-0.0791102)+x9*(0.187842)+x10*(-0.102019)+x11*(-0.0895597)+x12*(-0.117195)+x13*(-0.261887)+x14*(0.356671)+x15*(0.129811)+x16*(-0.213654)+x17*(-0.138725)+x18*(-0.24588)+x19*(-0.377858)+(0.0292081)); 
    h2 = tanh(x0*(0.396968)+x1*(0.397451)+x2*(-0.00100113)+x3*(-0.10822)+x4*(0.0348561)+x5*(-0.227092)+x6*(-0.0105611)+x7*(0.154221)+x8*(-0.238309)+x9*(-0.313261)+x10*(-0.329599)+x11*(-0.166764)+x12*(0.302987)+x13*(0.239214)+x14*(-0.215621)+x15*(-0.425465)+x16*(-0.326531)+x17*(-0.0748956)+x18*(-0.65069)+x19*(0.493982)+(-0.110234)); 
    h3 = tanh(x0*(-0.0685135)+x1*(0.294099)+x2*(0.0844838)+x3*(0.0909749)+x4*(-0.110003)+x5*(-0.549195)+x6*(-0.227107)+x7*(0.0406511)+x8*(-0.178284)+x9*(-0.137855)+x10*(0.380165)+x11*(-0.364578)+x12*(0.266647)+x13*(-0.478624)+x14*(0.196688)+x15*(-0.029437)+x16*(0.179928)+x17*(-0.339614)+x18*(-0.309605)+x19*(0.272447)+(-0.0280388)); 
    h4 = tanh(x0*(0.0103544)+x1*(0.237508)+x2*(0.140597)+x3*(0.33191)+x4*(-0.0718922)+x5*(-0.417576)+x6*(-0.138127)+x7*(-0.700076)+x8*(-0.464151)+x9*(-0.225834)+x10*(-0.298572)+x11*(-0.43301)+x12*(-0.302959)+x13*(-0.0842119)+x14*(0.0783974)+x15*(-0.363345)+x16*(-0.381641)+x17*(-0.28194)+x18*(-0.604001)+x19*(-0.248373)+(-0.0480111)); 
    h5 = tanh(x0*(-0.406242)+x1*(-0.202136)+x2*(0.0493499)+x3*(0.00973781)+x4*(0.310063)+x5*(0.355294)+x6*(0.0249382)+x7*(-0.272984)+x8*(-0.319958)+x9*(0.211681)+x10*(0.206407)+x11*(-0.16192)+x12*(-0.0512206)+x13*(0.1947)+x14*(0.0198451)+x15*(-0.1157)+x16*(0.0528255)+x17*(-0.22353)+x18*(-0.0315644)+x19*(-0.255323)+(-0.00698391)); 
    h6 = tanh(x0*(0.413214)+x1*(-0.22867)+x2*(-0.100268)+x3*(0.314557)+x4*(0.0094542)+x5*(-0.663872)+x6*(-0.0450858)+x7*(0.127902)+x8*(-0.369322)+x9*(-0.14482)+x10*(0.371997)+x11*(0.302855)+x12*(0.156296)+x13*(-0.224347)+x14*(0.363224)+x15*(-0.0507337)+x16*(-0.261751)+x17*(-0.186654)+x18*(-0.368961)+x19*(0.162435)+(0.0948861)); 
    h7 = tanh(x0*(0.409972)+x1*(0.311724)+x2*(-0.541791)+x3*(-0.434812)+x4*(-0.033129)+x5*(0.0630955)+x6*(0.0475688)+x7*(-0.214445)+x8*(0.0824788)+x9*(0.384339)+x10*(0.347103)+x11*(0.00147901)+x12*(0.275624)+x13*(0.367643)+x14*(0.142398)+x15*(-0.169487)+x16*(0.185484)+x17*(0.0350208)+x18*(-0.436413)+x19*(0.0390532)+(0.0534397)); 
    h8 = tanh(x0*(-0.312838)+x1*(-0.287808)+x2*(0.0263595)+x3*(-0.26433)+x4*(-0.385523)+x5*(-0.531114)+x6*(0.112829)+x7*(0.0657434)+x8*(-0.419771)+x9*(-0.367981)+x10*(0.0890669)+x11*(0.456188)+x12*(-0.120605)+x13*(-0.160979)+x14*(0.0766085)+x15*(-0.343766)+x16*(-0.229241)+x17*(-0.0956808)+x18*(-0.305125)+x19*(0.0176298)+(0.0343753)); 
    h9 = tanh(x0*(0.481862)+x1*(-0.0431022)+x2*(0.103835)+x3*(-0.144534)+x4*(0.510828)+x5*(-1.05507)+x6*(-0.301195)+x7*(0.31156)+x8*(-0.277085)+x9*(-0.491837)+x10*(0.647852)+x11*(0.604742)+x12*(-0.332796)+x13*(-0.203343)+x14*(0.372483)+x15*(-0.547002)+x16*(-0.21315)+x17*(-0.422441)+x18*(-0.462335)+x19*(-0.00959333)+(0.0261197)); 
}

void FNN_14_1_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(0.26621)+x1*(-0.284523)+x2*(-0.274779)+x3*(-0.301368)+x4*(0.168383)+x5*(0.442606)+x6*(0.451369)+x7*(0.311974)+x8*(0.384741)+x9*(-0.35267)+(0.00937486); 
    h1 = x0*(0.540854)+x1*(0.0150623)+x2*(-0.607784)+x3*(0.48453)+x4*(0.240825)+x5*(-0.242074)+x6*(0.381632)+x7*(-0.00802992)+x8*(-0.319839)+x9*(-0.353357)+(0.0126893); 
    h2 = x0*(0.525788)+x1*(-0.278697)+x2*(-0.48152)+x3*(-0.24919)+x4*(0.318777)+x5*(-0.562159)+x6*(0.0755709)+x7*(-0.479805)+x8*(0.343139)+x9*(-0.286456)+(0.0120677); 
}

void FNN_14_1(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_14_1_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_14_1_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_14_1_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_1(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_14_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void FNN_14_2_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(-0.156469)+x1*(0.0241276)+x2*(0.25525)+x3*(0.407244)+x4*(0.411159)+x5*(0.334048)+x6*(-0.234717)+x7*(-0.141813)+x8*(-0.0364193)+x9*(-0.0321587)+x10*(0.161964)+x11*(0.357791)+x12*(0.169365)+x13*(0.258529)+x14*(-0.396307)+x15*(-0.137608)+x16*(0.103595)+(-0.301382)); 
    h1 = tanh(x0*(-0.0658836)+x1*(0.440732)+x2*(-0.166816)+x3*(-0.020814)+x4*(0.0999837)+x5*(-0.652439)+x6*(-0.130491)+x7*(0.357558)+x8*(0.186137)+x9*(0.396058)+x10*(0.318347)+x11*(0.0372938)+x12*(0.308495)+x13*(0.11529)+x14*(0.162038)+x15*(-0.211411)+x16*(-0.391449)+(0.0913449)); 
    h2 = tanh(x0*(0.264047)+x1*(0.267901)+x2*(-0.203265)+x3*(-0.0487136)+x4*(-0.217009)+x5*(0.0718867)+x6*(-0.194433)+x7*(-0.323621)+x8*(0.244527)+x9*(0.289527)+x10*(-0.396702)+x11*(-0.0126101)+x12*(-0.26087)+x13*(-0.0846234)+x14*(-0.250261)+x15*(-0.0789539)+x16*(-0.240861)+(0.0280092)); 
    h3 = tanh(x0*(0.314176)+x1*(0.247783)+x2*(0.489224)+x3*(-0.0924392)+x4*(-0.0048374)+x5*(0.495877)+x6*(0.23775)+x7*(0.276894)+x8*(0.320593)+x9*(-0.358901)+x10*(-0.307478)+x11*(0.127753)+x12*(0.197023)+x13*(0.37001)+x14*(-0.0944939)+x15*(-0.296103)+x16*(-0.222757)+(-0.0247442)); 
    h4 = tanh(x0*(0.336052)+x1*(0.336641)+x2*(0.18184)+x3*(0.108124)+x4*(-0.0793848)+x5*(0.263373)+x6*(0.178581)+x7*(-0.600382)+x8*(0.280113)+x9*(-0.244489)+x10*(-0.180188)+x11*(0.023943)+x12*(0.128934)+x13*(-0.401375)+x14*(0.357544)+x15*(-0.234298)+x16*(0.172788)+(0.135337)); 
    h5 = tanh(x0*(-0.110941)+x1*(-0.189197)+x2*(-0.198363)+x3*(-0.186628)+x4*(0.293546)+x5*(0.0807067)+x6*(-0.355023)+x7*(0.301411)+x8*(-0.225506)+x9*(0.29151)+x10*(-0.262917)+x11*(-0.189453)+x12*(-0.372562)+x13*(-0.201915)+x14*(-0.0943029)+x15*(0.0216648)+x16*(-0.248868)+(0.183982)); 
    h6 = tanh(x0*(0.122697)+x1*(-0.364469)+x2*(-0.0486367)+x3*(0.172888)+x4*(0.00145382)+x5*(0.621667)+x6*(-0.106934)+x7*(0.180431)+x8*(0.086217)+x9*(-0.289233)+x10*(0.293018)+x11*(0.0731738)+x12*(0.195648)+x13*(-0.38836)+x14*(0.0931156)+x15*(0.172378)+x16*(-0.253319)+(-0.238817)); 
    h7 = tanh(x0*(0.138348)+x1*(0.285602)+x2*(-0.156526)+x3*(0.390697)+x4*(-0.467629)+x5*(-0.327)+x6*(-0.203288)+x7*(-0.00938488)+x8*(0.0255329)+x9*(0.10816)+x10*(0.188003)+x11*(0.399565)+x12*(0.34813)+x13*(-0.313643)+x14*(-0.0255436)+x15*(0.0709845)+x16*(0.0653609)+(0.132744)); 
    h8 = tanh(x0*(-0.0866208)+x1*(0.0870966)+x2*(-0.0550459)+x3*(0.337697)+x4*(-0.242709)+x5*(-0.0897643)+x6*(0.12899)+x7*(0.577405)+x8*(0.189326)+x9*(0.341861)+x10*(-0.0878393)+x11*(0.232923)+x12*(-0.128254)+x13*(-0.181387)+x14*(-0.298467)+x15*(-0.030797)+x16*(-0.111064)+(-0.0228791)); 
    h9 = tanh(x0*(-0.186135)+x1*(0.0613091)+x2*(-0.00981245)+x3*(0.385722)+x4*(-0.319758)+x5*(-0.457337)+x6*(0.383563)+x7*(-0.155921)+x8*(-0.276114)+x9*(0.340965)+x10*(-0.161423)+x11*(0.320601)+x12*(-0.155806)+x13*(0.265804)+x14*(0.375494)+x15*(-0.116816)+x16*(0.00354937)+(0.123181)); 
    h10 = tanh(x0*(0.0566977)+x1*(-0.43173)+x2*(-0.0236609)+x3*(0.0930486)+x4*(0.0117316)+x5*(-0.266789)+x6*(0.0495744)+x7*(-0.417413)+x8*(0.338246)+x9*(-0.336393)+x10*(-0.00100917)+x11*(-0.115185)+x12*(0.257716)+x13*(-0.28317)+x14*(-0.309577)+x15*(-0.346906)+x16*(-0.026008)+(0.340458)); 
    h11 = tanh(x0*(0.26375)+x1*(-0.303387)+x2*(0.090552)+x3*(-0.193393)+x4*(-0.204543)+x5*(0.462702)+x6*(0.130286)+x7*(0.0560667)+x8*(0.122887)+x9*(-0.252279)+x10*(-0.150673)+x11*(0.0794272)+x12*(-0.166646)+x13*(0.175463)+x14*(-0.0385866)+x15*(0.281286)+x16*(-0.218434)+(-0.39303)); 
    h12 = tanh(x0*(-0.371199)+x1*(-0.283447)+x2*(-0.388485)+x3*(-0.454426)+x4*(-0.0616792)+x5*(-0.427908)+x6*(0.0310568)+x7*(-0.150376)+x8*(0.328597)+x9*(-0.397859)+x10*(-0.0306576)+x11*(-0.237318)+x12*(0.144727)+x13*(-0.117259)+x14*(-0.0188645)+x15*(-0.0219797)+x16*(0.28828)+(-0.0675393)); 
    h13 = tanh(x0*(0.117351)+x1*(0.0541326)+x2*(0.365205)+x3*(-0.164999)+x4*(-0.0200128)+x5*(0.0647967)+x6*(0.085474)+x7*(0.106118)+x8*(-0.26737)+x9*(-0.0276988)+x10*(-0.318953)+x11*(0.131734)+x12*(0.182631)+x13*(0.108288)+x14*(0.374398)+x15*(-0.164607)+x16*(-0.322625)+(-0.256123)); 
    h14 = tanh(x0*(0.1586)+x1*(0.125746)+x2*(0.455098)+x3*(-0.185371)+x4*(0.573008)+x5*(0.21154)+x6*(-0.292491)+x7*(0.307351)+x8*(-0.137152)+x9*(-0.343782)+x10*(-0.263498)+x11*(0.00344971)+x12*(0.270285)+x13*(-0.352517)+x14*(-0.297379)+x15*(0.202991)+x16*(-0.12434)+(-0.262617)); 
    h15 = tanh(x0*(-0.0189026)+x1*(-0.133999)+x2*(-0.179321)+x3*(-0.18114)+x4*(0.118644)+x5*(-0.340736)+x6*(0.140847)+x7*(0.236147)+x8*(-0.0938698)+x9*(-0.353825)+x10*(-0.327108)+x11*(0.119696)+x12*(-0.0468433)+x13*(0.09155)+x14*(0.0821223)+x15*(-0.153664)+x16*(0.316414)+(0.154867)); 
    h16 = tanh(x0*(0.246438)+x1*(-0.387227)+x2*(-0.494643)+x3*(0.132704)+x4*(0.0392344)+x5*(-0.332089)+x6*(0.00498671)+x7*(-0.152733)+x8*(-0.0940918)+x9*(-0.131414)+x10*(-0.281936)+x11*(-0.350019)+x12*(-0.0873245)+x13*(-0.149556)+x14*(-0.0812948)+x15*(0.0956181)+x16*(-0.163228)+(-0.163062)); 
    h17 = tanh(x0*(0.101363)+x1*(0.158149)+x2*(-0.119613)+x3*(0.315506)+x4*(-0.237186)+x5*(0.0148921)+x6*(0.00921441)+x7*(0.203765)+x8*(0.188394)+x9*(-0.034414)+x10*(0.298262)+x11*(0.0533206)+x12*(0.0524603)+x13*(0.0963372)+x14*(0.31189)+x15*(0.200202)+x16*(-0.316944)+(-0.130548)); 
    h18 = tanh(x0*(-0.534919)+x1*(0.132191)+x2*(-0.0511046)+x3*(0.386124)+x4*(-0.0710637)+x5*(0.540661)+x6*(0.0743406)+x7*(0.0266047)+x8*(0.433536)+x9*(-0.213771)+x10*(0.390614)+x11*(-0.130064)+x12*(0.0922619)+x13*(0.115133)+x14*(-0.339815)+x15*(0.351292)+x16*(-0.257873)+(-0.504432)); 
    h19 = tanh(x0*(0.194768)+x1*(-0.30595)+x2*(-0.536608)+x3*(0.19297)+x4*(0.0177858)+x5*(-0.509854)+x6*(-0.142065)+x7*(-0.0735145)+x8*(0.0534228)+x9*(0.372092)+x10*(0.18063)+x11*(-0.376478)+x12*(-0.105531)+x13*(-0.0748791)+x14*(0.155496)+x15*(-0.243081)+x16*(0.24454)+(0.0034389)); 
}

void FNN_14_2_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(0.076953)+x1*(0.204249)+x2*(-0.235524)+x3*(-0.320989)+x4*(0.0847828)+x5*(-0.277213)+x6*(0.184533)+x7*(0.0402401)+x8*(-0.352397)+x9*(-0.0300584)+x10*(0.642401)+x11*(0.359899)+x12*(0.153861)+x13*(0.290355)+x14*(-0.2099)+x15*(-0.0307446)+x16*(-0.140312)+x17*(-0.19097)+x18*(-0.124661)+x19*(0.237361)+(0.132682)); 
    h1 = tanh(x0*(0.386238)+x1*(-0.169427)+x2*(0.0856543)+x3*(-0.382921)+x4*(0.0459308)+x5*(-0.315009)+x6*(0.455005)+x7*(-0.191055)+x8*(0.173579)+x9*(-0.404289)+x10*(-0.794434)+x11*(0.432827)+x12*(0.1255)+x13*(0.499506)+x14*(-0.0955899)+x15*(-0.0676758)+x16*(0.304098)+x17*(0.165805)+x18*(0.141631)+x19*(-0.131978)+(-0.148484)); 
    h2 = tanh(x0*(-0.474426)+x1*(0.918832)+x2*(-0.230841)+x3*(-0.47875)+x4*(0.234157)+x5*(0.0367123)+x6*(-0.56835)+x7*(0.0124445)+x8*(0.433741)+x9*(0.0808696)+x10*(0.761838)+x11*(-0.612633)+x12*(-0.091174)+x13*(-0.678003)+x14*(-0.468332)+x15*(0.307194)+x16*(0.19714)+x17*(0.0292013)+x18*(-0.56424)+x19*(0.191955)+(0.0212429)); 
    h3 = tanh(x0*(0.0886957)+x1*(-0.195673)+x2*(-0.0741409)+x3*(0.12934)+x4*(-0.0473884)+x5*(-0.223368)+x6*(-0.143947)+x7*(-0.36008)+x8*(0.303382)+x9*(-0.111907)+x10*(-0.514666)+x11*(0.148799)+x12*(0.101694)+x13*(0.610491)+x14*(0.492126)+x15*(-0.396474)+x16*(0.332361)+x17*(0.0142212)+x18*(0.350956)+x19*(0.297092)+(-0.102896)); 
    h4 = tanh(x0*(0.607165)+x1*(-0.354965)+x2*(0.045705)+x3*(0.488621)+x4*(-0.0435965)+x5*(-0.0424443)+x6*(0.233568)+x7*(0.265994)+x8*(0.399822)+x9*(0.146598)+x10*(-0.133928)+x11*(-0.180461)+x12*(0.274633)+x13*(0.596212)+x14*(0.356049)+x15*(0.0108627)+x16*(0.0406694)+x17*(0.40368)+x18*(-0.248358)+x19*(-0.19899)+(0.027186)); 
    h5 = tanh(x0*(0.0758602)+x1*(0.998373)+x2*(-0.202304)+x3*(-0.175582)+x4*(0.301268)+x5*(0.210811)+x6*(-0.262245)+x7*(0.318548)+x8*(0.287828)+x9*(0.345528)+x10*(0.607705)+x11*(0.355503)+x12*(-0.00906044)+x13*(-0.0650953)+x14*(-0.0393608)+x15*(0.503761)+x16*(0.24143)+x17*(-0.0981432)+x18*(-0.524959)+x19*(0.106647)+(-0.0629922)); 
    h6 = tanh(x0*(-0.344392)+x1*(-0.183076)+x2*(0.0607488)+x3*(-0.310037)+x4*(-0.284469)+x5*(-0.124804)+x6*(-0.438718)+x7*(-0.355693)+x8*(-0.0966784)+x9*(-0.0486007)+x10*(-0.184299)+x11*(0.212985)+x12*(0.275944)+x13*(0.607173)+x14*(-0.082489)+x15*(0.194002)+x16*(-0.0103133)+x17*(0.198297)+x18*(-0.313182)+x19*(0.0282767)+(-0.0981905)); 
    h7 = tanh(x0*(-0.108658)+x1*(-0.556793)+x2*(0.1718)+x3*(-0.141892)+x4*(0.508456)+x5*(-0.0797341)+x6*(0.254131)+x7*(0.215766)+x8*(-0.24504)+x9*(-0.134096)+x10*(0.309427)+x11*(-0.49653)+x12*(-0.0824131)+x13*(-0.499538)+x14*(-0.241997)+x15*(-0.200031)+x16*(-0.516771)+x17*(0.0479113)+x18*(-0.178806)+x19*(-0.0218317)+(0.00262452)); 
    h8 = tanh(x0*(-0.0751791)+x1*(-0.0734001)+x2*(-0.310908)+x3*(0.0963717)+x4*(0.139349)+x5*(0.138676)+x6*(0.15234)+x7*(0.361458)+x8*(-0.0663716)+x9*(-0.0705612)+x10*(-0.1435)+x11*(0.413782)+x12*(-0.101298)+x13*(-0.112507)+x14*(0.103321)+x15*(-0.0333348)+x16*(-0.0668765)+x17*(-0.053578)+x18*(-0.148051)+x19*(0.102728)+(-0.0195134)); 
    h9 = tanh(x0*(0.0507041)+x1*(0.0665351)+x2*(-0.372517)+x3*(0.115184)+x4*(0.0966211)+x5*(0.19884)+x6*(0.302515)+x7*(0.36811)+x8*(0.424664)+x9*(-0.129581)+x10*(-0.325684)+x11*(-0.33067)+x12*(0.247194)+x13*(0.2189)+x14*(0.280474)+x15*(0.147155)+x16*(-0.287838)+x17*(0.00192377)+x18*(0.408231)+x19*(0.360048)+(-0.00091597)); 
}

void FNN_14_2_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(0.470442)+x1*(-0.799574)+x2*(-0.232262)+x3*(-0.126487)+x4*(-0.0736069)+x5*(-0.342959)+x6*(-0.143329)+x7*(-0.217933)+x8*(0.575966)+x9*(-0.132606)+(0.0771569); 
    h1 = x0*(-0.495015)+x1*(-0.251684)+x2*(-0.419663)+x3*(-0.478143)+x4*(-0.13801)+x5*(0.150034)+x6*(-0.253488)+x7*(-0.0952267)+x8*(-0.419869)+x9*(-0.416627)+(0.013997); 
    h2 = x0*(0.452075)+x1*(-0.37298)+x2*(-0.185778)+x3*(-0.520672)+x4*(0.467074)+x5*(-0.328495)+x6*(0.11342)+x7*(-0.112637)+x8*(-0.400144)+x9*(0.128298)+(0.0483506); 
}

void FNN_14_2(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_14_2_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_14_2_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_14_2_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_2(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_14_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void ModelKDTree_0(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    if ( x6 < 0.4 ) {
        ModelKDTree_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
    }
    else if ( x6 < 0.6 ) {
        float left_h0;
        float left_h1;
        float left_h2;
        float right_h0;
        float right_h1;
        float right_h2;
        
        ModelKDTree_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,left_h0,left_h1,left_h2);
        ModelKDTree_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,right_h0,right_h1,right_h2);
        
        float t = 0.5 + ( x6 - 0.5 ) / 0.2;
        t = ((6*t - 15)*t + 10) * t*t*t;
        h0 = ( 1 - t ) * left_h0 + t * right_h0;
        h1 = ( 1 - t ) * left_h1 + t * right_h1;
        h2 = ( 1 - t ) * left_h2 + t * right_h2;
    }
    else {
        ModelKDTree_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
    }
}

void Model(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,    
out float h0,out float h1,out float h2)
{
    // map to unit hyperbox
    x0 = (x0 - (0.104431)) / (0.895569);
    x1 = (x1 - (0.104431)) / (0.895569);
    x2 = (x2 - (0.104431)) / (0.895569);
    x3 = (x3 - (-0.999512)) / (1.99902);
    x4 = (x4 - (0)) / (1e-06);
    x5 = (x5 - (-0.999512)) / (1.99902);
    x6 = (x6 - (-0.995605)) / (1.98828);
    x7 = (x7 - (0.028336)) / (0.971176);
    x8 = (x8 - (-0.999023)) / (1.98926);
    x9 = (x9 - (0)) / (1);
    x10 = (x10 - (1)) / (1);
    x11 = (x11 - (0)) / (1);
    x12 = (x12 - (1)) / (1);
    x13 = (x13 - (1)) / (1);
    x14 = (x14 - (1)) / (1);
    x15 = (x15 - (20)) / (1);
    x16 = (x16 - (1)) / (1);

    ModelKDTree_0(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);

    // map back
    h0 = h0 * (0.895569) + (0.104431);
    h1 = h1 * (0.895569) + (0.104431);
    h2 = h2 * (0.895569) + (0.104431);
}

