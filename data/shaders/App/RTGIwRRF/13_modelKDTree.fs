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

void FNN_13_1_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(-0.398509)+x1*(0.23654)+x2*(-0.353871)+x3*(-0.171666)+x4*(-0.0293566)+x5*(0.0532901)+x6*(-0.183153)+x7*(0.458137)+x8*(-0.0290805)+x9*(0.0632506)+x10*(0.0193981)+x11*(-0.188002)+x12*(-0.326526)+x13*(0.172748)+x14*(0.189161)+x15*(0.309826)+x16*(-0.315702)+(0.0720395)); 
    h1 = tanh(x0*(0.0272103)+x1*(0.19278)+x2*(-0.372891)+x3*(0.38648)+x4*(-0.135019)+x5*(-0.717194)+x6*(0.141621)+x7*(-0.0992118)+x8*(0.0447242)+x9*(0.240949)+x10*(-0.213851)+x11*(-0.244896)+x12*(0.189415)+x13*(0.127345)+x14*(0.100993)+x15*(0.203776)+x16*(-0.112503)+(0.375432)); 
    h2 = tanh(x0*(0.473453)+x1*(-0.131968)+x2*(-0.242892)+x3*(-0.34074)+x4*(-0.0735253)+x5*(0.0105475)+x6*(0.180345)+x7*(0.0154111)+x8*(0.156307)+x9*(0.340452)+x10*(-0.0491439)+x11*(-0.000936091)+x12*(-0.00606358)+x13*(-0.231761)+x14*(-0.0451743)+x15*(0.0277229)+x16*(0.205329)+(-0.0116093)); 
    h3 = tanh(x0*(-0.0865221)+x1*(0.0596052)+x2*(0.335475)+x3*(-0.0910989)+x4*(0.0755201)+x5*(0.308445)+x6*(-0.264048)+x7*(-0.166257)+x8*(-0.00428972)+x9*(-0.38783)+x10*(-0.296733)+x11*(0.0929032)+x12*(-0.36987)+x13*(0.333261)+x14*(-0.237567)+x15*(0.103047)+x16*(-0.0414083)+(-0.0730847)); 
    h4 = tanh(x0*(-0.0331062)+x1*(-0.195217)+x2*(-0.287481)+x3*(0.0194228)+x4*(0.0839803)+x5*(-0.344245)+x6*(0.190751)+x7*(0.257212)+x8*(0.120249)+x9*(0.309797)+x10*(-0.371357)+x11*(0.374924)+x12*(0.25761)+x13*(-0.402546)+x14*(0.33674)+x15*(0.34265)+x16*(-0.324526)+(0.161383)); 
    h5 = tanh(x0*(0.148875)+x1*(0.323626)+x2*(-0.37704)+x3*(-0.0101995)+x4*(-0.0878958)+x5*(0.337653)+x6*(0.18212)+x7*(0.0384823)+x8*(0.0132378)+x9*(0.0925251)+x10*(0.243266)+x11*(0.285999)+x12*(-0.0298639)+x13*(-0.209068)+x14*(0.033969)+x15*(0.387687)+x16*(0.151647)+(-0.137237)); 
    h6 = tanh(x0*(0.0580537)+x1*(0.23875)+x2*(-0.370387)+x3*(-0.136212)+x4*(-0.244581)+x5*(-0.0279719)+x6*(0.0223837)+x7*(0.149472)+x8*(0.271872)+x9*(-0.378062)+x10*(0.183414)+x11*(-0.143601)+x12*(0.0448472)+x13*(-0.0748432)+x14*(0.22791)+x15*(0.0966482)+x16*(0.270401)+(0.0466072)); 
    h7 = tanh(x0*(-0.326922)+x1*(-0.562612)+x2*(-0.649203)+x3*(-0.346375)+x4*(0.0969532)+x5*(-0.393229)+x6*(-0.311825)+x7*(0.114808)+x8*(-0.203597)+x9*(-0.147477)+x10*(-0.165545)+x11*(0.288072)+x12*(-0.189687)+x13*(0.322422)+x14*(-0.0107105)+x15*(-0.117886)+x16*(-0.136951)+(-0.0466067)); 
    h8 = tanh(x0*(0.229799)+x1*(0.0770383)+x2*(-0.166336)+x3*(-0.0322403)+x4*(-0.0235363)+x5*(0.341297)+x6*(0.139676)+x7*(0.210823)+x8*(-0.217953)+x9*(-0.28384)+x10*(0.278677)+x11*(-0.254715)+x12*(-0.125584)+x13*(0.219688)+x14*(0.238161)+x15*(0.124967)+x16*(0.0305142)+(-0.276834)); 
    h9 = tanh(x0*(-0.235581)+x1*(0.459871)+x2*(-0.110063)+x3*(-0.131174)+x4*(-0.179281)+x5*(0.575426)+x6*(0.104677)+x7*(-0.122017)+x8*(-0.179485)+x9*(-0.146239)+x10*(-0.153231)+x11*(-0.0342304)+x12*(-0.175902)+x13*(-0.367955)+x14*(0.21603)+x15*(-0.309516)+x16*(0.270643)+(-0.151777)); 
    h10 = tanh(x0*(0.238192)+x1*(0.200598)+x2*(0.0025412)+x3*(-0.150313)+x4*(0.0540556)+x5*(0.11294)+x6*(0.169579)+x7*(-0.196427)+x8*(0.133888)+x9*(0.226085)+x10*(-0.259549)+x11*(-0.0955423)+x12*(-0.324157)+x13*(-0.180964)+x14*(-0.358616)+x15*(-0.00864086)+x16*(-0.333739)+(-0.114832)); 
    h11 = tanh(x0*(-0.0527178)+x1*(0.257914)+x2*(0.178506)+x3*(-0.089976)+x4*(-0.195211)+x5*(0.770589)+x6*(-0.181903)+x7*(0.374292)+x8*(0.0267424)+x9*(0.113857)+x10*(-0.112454)+x11*(-0.255276)+x12*(0.251314)+x13*(0.401931)+x14*(-0.330805)+x15*(-0.275572)+x16*(0.162341)+(-0.434866)); 
    h12 = tanh(x0*(0.0632637)+x1*(0.178481)+x2*(0.107964)+x3*(-0.393296)+x4*(-0.220551)+x5*(-0.388208)+x6*(0.282944)+x7*(-0.012047)+x8*(0.298258)+x9*(-0.333061)+x10*(-0.0101947)+x11*(-0.242125)+x12*(0.398666)+x13*(-0.19035)+x14*(-0.19709)+x15*(-0.104686)+x16*(-0.150691)+(0.157248)); 
    h13 = tanh(x0*(0.213884)+x1*(-0.196231)+x2*(-0.0568883)+x3*(-0.468172)+x4*(0.13825)+x5*(-0.401266)+x6*(-0.0311642)+x7*(0.329349)+x8*(-0.290753)+x9*(0.146796)+x10*(0.0593357)+x11*(0.105164)+x12*(0.000907093)+x13*(-0.24293)+x14*(0.0122587)+x15*(0.0239297)+x16*(-0.0546781)+(0.38155)); 
    h14 = tanh(x0*(-0.498705)+x1*(0.21531)+x2*(0.0518318)+x3*(0.248249)+x4*(-0.0580809)+x5*(0.321157)+x6*(0.0203853)+x7*(-0.0241319)+x8*(-0.309809)+x9*(0.0318691)+x10*(-0.283584)+x11*(-0.169943)+x12*(-0.194424)+x13*(0.0427129)+x14*(0.282438)+x15*(-0.122185)+x16*(0.0523116)+(-0.0639988)); 
    h15 = tanh(x0*(-0.203022)+x1*(0.486719)+x2*(-0.217554)+x3*(0.430548)+x4*(-0.0370743)+x5*(-0.235004)+x6*(-0.280024)+x7*(-0.174567)+x8*(-0.179791)+x9*(0.0409457)+x10*(0.166776)+x11*(0.104514)+x12*(0.264915)+x13*(0.0895337)+x14*(-0.0677916)+x15*(-0.0197237)+x16*(0.0884236)+(-0.0619358)); 
    h16 = tanh(x0*(0.395967)+x1*(-0.192998)+x2*(0.435623)+x3*(0.0630998)+x4*(0.0593215)+x5*(-0.0632879)+x6*(0.0259509)+x7*(-0.1059)+x8*(-0.136831)+x9*(0.273097)+x10*(0.322598)+x11*(-0.336278)+x12*(-0.0602511)+x13*(0.36806)+x14*(-0.0715713)+x15*(-0.0263189)+x16*(0.287038)+(-0.162431)); 
    h17 = tanh(x0*(-0.13718)+x1*(-0.0260967)+x2*(0.120603)+x3*(0.219364)+x4*(-0.0525797)+x5*(-0.229904)+x6*(-0.0768779)+x7*(-0.161572)+x8*(-0.133519)+x9*(-0.134006)+x10*(-0.172331)+x11*(0.381019)+x12*(-0.389268)+x13*(0.340026)+x14*(0.359575)+x15*(0.0470586)+x16*(0.36808)+(0.172875)); 
    h18 = tanh(x0*(-0.00118952)+x1*(0.164188)+x2*(-0.136706)+x3*(-0.0970045)+x4*(-0.17267)+x5*(-0.485785)+x6*(-0.137809)+x7*(0.504092)+x8*(-0.491415)+x9*(0.204868)+x10*(-0.288352)+x11*(0.0488927)+x12*(-0.270382)+x13*(0.294751)+x14*(-0.0529021)+x15*(0.117638)+x16*(-0.100123)+(0.409317)); 
    h19 = tanh(x0*(-0.463693)+x1*(-0.0730473)+x2*(0.206829)+x3*(0.0952251)+x4*(0.0429164)+x5*(0.38009)+x6*(0.371231)+x7*(0.207111)+x8*(0.0941664)+x9*(-0.135443)+x10*(-0.0800452)+x11*(0.387419)+x12*(0.0271707)+x13*(0.390335)+x14*(0.322434)+x15*(0.0762193)+x16*(-0.0880691)+(-0.0308728)); 
}

void FNN_13_1_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(-0.109969)+x1*(0.532958)+x2*(0.0318051)+x3*(-0.06174)+x4*(0.344792)+x5*(-0.175315)+x6*(0.0702471)+x7*(0.193845)+x8*(-0.427057)+x9*(-0.521553)+x10*(-0.0172897)+x11*(-0.768092)+x12*(0.288444)+x13*(0.426429)+x14*(-0.267003)+x15*(0.164954)+x16*(-0.253478)+x17*(0.231588)+x18*(0.606896)+x19*(-0.323088)+(0.100628)); 
    h1 = tanh(x0*(-0.100016)+x1*(0.00102624)+x2*(-0.113469)+x3*(0.136621)+x4*(-0.115275)+x5*(0.312076)+x6*(0.0193745)+x7*(-0.142432)+x8*(-0.0737021)+x9*(0.407887)+x10*(-0.127866)+x11*(0.277226)+x12*(0.29724)+x13*(0.0752409)+x14*(-0.0182018)+x15*(-0.114033)+x16*(0.0988385)+x17*(0.17182)+x18*(0.212928)+x19*(-0.370329)+(0.0416181)); 
    h2 = tanh(x0*(-0.21495)+x1*(0.466059)+x2*(0.097787)+x3*(-0.0648263)+x4*(0.0477897)+x5*(0.361292)+x6*(-0.0643995)+x7*(0.0359814)+x8*(-0.225574)+x9*(-0.857354)+x10*(-0.0872194)+x11*(-0.267229)+x12*(0.0508215)+x13*(0.302031)+x14*(-0.158766)+x15*(0.125489)+x16*(0.315841)+x17*(-0.370055)+x18*(0.289574)+x19*(0.0848992)+(0.116108)); 
    h3 = tanh(x0*(-0.237742)+x1*(0.438378)+x2*(-0.0197734)+x3*(0.0867855)+x4*(-0.501498)+x5*(0.359168)+x6*(-0.290387)+x7*(-0.230627)+x8*(0.000105107)+x9*(-0.12189)+x10*(-0.410367)+x11*(-0.113408)+x12*(-0.121249)+x13*(-0.500312)+x14*(-0.217946)+x15*(-0.181071)+x16*(0.267981)+x17*(-0.251758)+x18*(0.10529)+x19*(0.163743)+(-0.0505136)); 
    h4 = tanh(x0*(-0.0912696)+x1*(0.145087)+x2*(0.359468)+x3*(-0.133326)+x4*(-0.423809)+x5*(0.326717)+x6*(0.140966)+x7*(-0.127618)+x8*(-0.116725)+x9*(-0.222794)+x10*(-0.185809)+x11*(0.412709)+x12*(-0.308434)+x13*(0.556889)+x14*(-0.26099)+x15*(-0.445143)+x16*(-0.15331)+x17*(-0.0101977)+x18*(0.25276)+x19*(0.15481)+(-0.0557593)); 
    h5 = tanh(x0*(-0.31984)+x1*(-0.241543)+x2*(-0.00625517)+x3*(0.454042)+x4*(-0.188143)+x5*(0.114511)+x6*(0.23919)+x7*(0.446867)+x8*(0.511718)+x9*(0.323603)+x10*(-0.0400063)+x11*(0.37079)+x12*(0.0929459)+x13*(-0.320173)+x14*(-0.191292)+x15*(0.036062)+x16*(0.245336)+x17*(0.0610987)+x18*(-0.0890803)+x19*(-0.0650075)+(-0.0612091)); 
    h6 = tanh(x0*(0.240002)+x1*(-0.0477549)+x2*(-0.608822)+x3*(-0.313487)+x4*(-0.1857)+x5*(-0.0288685)+x6*(0.0640616)+x7*(-0.277127)+x8*(0.2421)+x9*(-0.239698)+x10*(0.338644)+x11*(-0.275102)+x12*(0.341603)+x13*(-0.381535)+x14*(-0.0605339)+x15*(-0.239225)+x16*(0.413609)+x17*(-0.290148)+x18*(0.427568)+x19*(-0.477759)+(-0.0443708)); 
    h7 = tanh(x0*(0.411179)+x1*(-0.198096)+x2*(-0.252873)+x3*(0.0884828)+x4*(-0.327664)+x5*(0.33646)+x6*(0.202644)+x7*(-0.0948561)+x8*(0.262875)+x9*(0.0336234)+x10*(0.183384)+x11*(0.165193)+x12*(0.314992)+x13*(0.196877)+x14*(0.0660237)+x15*(0.284922)+x16*(-0.0303677)+x17*(0.20887)+x18*(0.00569136)+x19*(0.300813)+(-0.0135222)); 
    h8 = tanh(x0*(-0.0770053)+x1*(0.24612)+x2*(0.192929)+x3*(0.150287)+x4*(0.0709032)+x5*(0.428488)+x6*(0.000637517)+x7*(-0.389928)+x8*(0.223896)+x9*(0.0982101)+x10*(0.392773)+x11*(0.348449)+x12*(-0.466024)+x13*(-0.0326482)+x14*(0.0116777)+x15*(0.160018)+x16*(-0.463365)+x17*(0.311599)+x18*(-0.156845)+x19*(0.316648)+(0.0521296)); 
    h9 = tanh(x0*(0.337659)+x1*(0.123024)+x2*(-0.0825339)+x3*(0.183323)+x4*(0.280628)+x5*(-0.374661)+x6*(0.219172)+x7*(-0.335169)+x8*(0.0697794)+x9*(-0.44071)+x10*(-0.357135)+x11*(-0.443442)+x12*(0.0852196)+x13*(0.0377909)+x14*(-0.31864)+x15*(-0.129698)+x16*(-0.169353)+x17*(0.152153)+x18*(0.536442)+x19*(0.119599)+(0.0492609)); 
}

void FNN_13_1_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(-0.376721)+x1*(0.318846)+x2*(0.230413)+x3*(-0.680954)+x4*(-0.197107)+x5*(-0.212714)+x6*(0.128254)+x7*(-0.215597)+x8*(0.48679)+x9*(0.300683)+(0.0447288); 
    h1 = x0*(-0.349062)+x1*(-0.0116537)+x2*(-0.485141)+x3*(0.0268503)+x4*(0.25323)+x5*(-0.524946)+x6*(0.551286)+x7*(-0.480378)+x8*(0.018813)+x9*(0.41064)+(0.0286316); 
    h2 = x0*(-0.419492)+x1*(0.0678237)+x2*(0.339103)+x3*(0.392229)+x4*(-0.0648393)+x5*(-0.236322)+x6*(-0.516592)+x7*(0.523405)+x8*(-0.260939)+x9*(0.49687)+(0.0119947); 
}

void FNN_13_1(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_13_1_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_13_1_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_13_1_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_1(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_13_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void FNN_13_2_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(0.162099)+x1*(-0.0147091)+x2*(-0.302598)+x3*(-0.301014)+x4*(0.37221)+x5*(0.133991)+x6*(0.166959)+x7*(0.0739351)+x8*(-0.183997)+x9*(-0.393817)+x10*(-0.0298296)+x11*(0.125637)+x12*(-0.316739)+x13*(0.0796383)+x14*(-0.142001)+x15*(0.344049)+x16*(-0.202303)+(-0.0479021)); 
    h1 = tanh(x0*(-0.0107066)+x1*(-0.103856)+x2*(0.256534)+x3*(-0.331962)+x4*(-0.118028)+x5*(0.483119)+x6*(-0.147842)+x7*(-0.165119)+x8*(0.106419)+x9*(0.199423)+x10*(-0.043943)+x11*(-0.0824555)+x12*(0.399582)+x13*(0.152758)+x14*(-0.195549)+x15*(0.146078)+x16*(-0.308042)+(0.0571943)); 
    h2 = tanh(x0*(0.155358)+x1*(-0.389669)+x2*(-0.224746)+x3*(-0.0599431)+x4*(-0.190723)+x5*(-0.206307)+x6*(0.255579)+x7*(0.399892)+x8*(-0.0348786)+x9*(0.259275)+x10*(-0.276059)+x11*(-0.156366)+x12*(0.078632)+x13*(0.109635)+x14*(-0.217193)+x15*(-0.0316865)+x16*(0.175497)+(0.117867)); 
    h3 = tanh(x0*(0.0137784)+x1*(-0.275445)+x2*(-0.213956)+x3*(0.122325)+x4*(0.271687)+x5*(-0.47432)+x6*(0.208748)+x7*(0.250934)+x8*(-0.289853)+x9*(-0.148956)+x10*(0.0137056)+x11*(-0.117812)+x12*(-0.000169933)+x13*(0.292251)+x14*(-0.238877)+x15*(0.000301361)+x16*(0.149744)+(0.0371746)); 
    h4 = tanh(x0*(0.390024)+x1*(0.0600987)+x2*(-0.066544)+x3*(-0.0960902)+x4*(0.22535)+x5*(-0.369686)+x6*(0.2786)+x7*(-0.0934846)+x8*(0.241503)+x9*(-0.0264439)+x10*(-0.393586)+x11*(0.154907)+x12*(-0.302008)+x13*(0.202506)+x14*(0.381432)+x15*(-0.0518939)+x16*(0.210732)+(-0.0624513)); 
    h5 = tanh(x0*(-0.0752308)+x1*(0.119506)+x2*(-0.466268)+x3*(-0.130952)+x4*(0.463622)+x5*(-0.60761)+x6*(-0.189432)+x7*(0.117349)+x8*(-0.0189225)+x9*(-0.0875787)+x10*(-0.275859)+x11*(-0.071739)+x12*(0.039978)+x13*(-0.175678)+x14*(0.153341)+x15*(0.214216)+x16*(-0.247268)+(0.305012)); 
    h6 = tanh(x0*(0.0813095)+x1*(0.380491)+x2*(-0.260681)+x3*(0.057833)+x4*(-0.383437)+x5*(0.170175)+x6*(-0.136877)+x7*(0.0488665)+x8*(0.243139)+x9*(0.0149153)+x10*(-0.324756)+x11*(-0.193006)+x12*(0.204312)+x13*(-0.108328)+x14*(-0.132008)+x15*(0.0623895)+x16*(0.398261)+(0.00098286)); 
    h7 = tanh(x0*(-0.202198)+x1*(0.00705207)+x2*(0.26439)+x3*(-0.112694)+x4*(0.411479)+x5*(-0.559188)+x6*(-0.330721)+x7*(-0.000658788)+x8*(0.518089)+x9*(0.0288799)+x10*(0.393048)+x11*(-0.222322)+x12*(0.0866188)+x13*(0.108999)+x14*(-0.386294)+x15*(-0.23087)+x16*(-0.140996)+(0.179102)); 
    h8 = tanh(x0*(0.232545)+x1*(-0.239549)+x2*(0.114069)+x3*(-0.178726)+x4*(0.249934)+x5*(0.451049)+x6*(0.193842)+x7*(-0.136583)+x8*(0.120988)+x9*(-0.368528)+x10*(-0.0880303)+x11*(-0.054224)+x12*(0.103479)+x13*(-0.0829001)+x14*(-0.0886613)+x15*(-0.270284)+x16*(-0.302644)+(-0.120945)); 
    h9 = tanh(x0*(0.23166)+x1*(-0.148303)+x2*(-0.391384)+x3*(-0.0324446)+x4*(-0.0185033)+x5*(-0.563935)+x6*(0.377605)+x7*(-0.0211778)+x8*(-0.139453)+x9*(-0.381509)+x10*(0.0562015)+x11*(-0.108972)+x12*(-0.134315)+x13*(0.397427)+x14*(-0.21081)+x15*(0.119093)+x16*(0.275378)+(0.279554)); 
    h10 = tanh(x0*(-0.289468)+x1*(-0.571053)+x2*(-0.405299)+x3*(0.324115)+x4*(-0.196351)+x5*(-0.213468)+x6*(0.0793155)+x7*(-0.0262677)+x8*(-0.0620538)+x9*(-0.092554)+x10*(0.106641)+x11*(-0.394512)+x12*(-0.391231)+x13*(0.316018)+x14*(0.299848)+x15*(0.385579)+x16*(0.000717282)+(-0.0315503)); 
    h11 = tanh(x0*(-0.184637)+x1*(-0.154706)+x2*(0.290369)+x3*(-0.113122)+x4*(-0.300083)+x5*(-0.205226)+x6*(-0.059097)+x7*(0.320865)+x8*(0.189959)+x9*(-0.0696694)+x10*(-0.147792)+x11*(-0.346256)+x12*(0.266769)+x13*(-0.376485)+x14*(0.301123)+x15*(-0.278685)+x16*(0.320698)+(-0.0014236)); 
    h12 = tanh(x0*(-0.0131014)+x1*(0.624097)+x2*(0.192831)+x3*(0.0595347)+x4*(0.0982741)+x5*(0.265344)+x6*(0.0188347)+x7*(-0.098058)+x8*(-0.388063)+x9*(0.373001)+x10*(-0.0306422)+x11*(0.173399)+x12*(0.0636789)+x13*(-0.199223)+x14*(-0.376292)+x15*(-0.064756)+x16*(-0.251016)+(-0.57267)); 
    h13 = tanh(x0*(0.328592)+x1*(0.0480567)+x2*(-0.215622)+x3*(0.243405)+x4*(0.0434147)+x5*(-0.0860325)+x6*(-0.286707)+x7*(0.309344)+x8*(-0.00620826)+x9*(-0.0534881)+x10*(0.251485)+x11*(0.293992)+x12*(0.263301)+x13*(-0.398695)+x14*(-0.355277)+x15*(0.133539)+x16*(-0.143236)+(0.0376153)); 
    h14 = tanh(x0*(-0.00421997)+x1*(-0.0130357)+x2*(-0.0256953)+x3*(0.0041674)+x4*(0.0633766)+x5*(0.443417)+x6*(-0.069697)+x7*(0.27055)+x8*(0.0470742)+x9*(-0.214173)+x10*(-0.28443)+x11*(0.0193984)+x12*(-0.350379)+x13*(-0.256731)+x14*(0.0759601)+x15*(0.339901)+x16*(-0.178189)+(-0.307489)); 
    h15 = tanh(x0*(-0.260026)+x1*(-0.00214193)+x2*(-0.265306)+x3*(0.0806571)+x4*(0.277373)+x5*(0.498969)+x6*(-0.0368824)+x7*(0.162701)+x8*(-0.0893367)+x9*(0.312177)+x10*(-0.312438)+x11*(-0.363689)+x12*(0.272577)+x13*(-0.193752)+x14*(-0.224892)+x15*(0.400576)+x16*(0.402162)+(0.0170791)); 
    h16 = tanh(x0*(0.144406)+x1*(0.00362786)+x2*(-0.0685591)+x3*(-0.185749)+x4*(0.33446)+x5*(0.470359)+x6*(-0.0514142)+x7*(-0.0617148)+x8*(0.259919)+x9*(-0.281635)+x10*(0.37209)+x11*(-0.10776)+x12*(-0.33971)+x13*(0.159195)+x14*(0.347546)+x15*(0.332632)+x16*(-0.35795)+(-0.104309)); 
    h17 = tanh(x0*(0.104897)+x1*(-0.0849368)+x2*(0.124788)+x3*(0.243114)+x4*(-0.313071)+x5*(-0.160333)+x6*(0.111093)+x7*(0.272885)+x8*(0.154189)+x9*(-0.35436)+x10*(0.139877)+x11*(0.309675)+x12*(-0.137266)+x13*(-0.284172)+x14*(-0.244411)+x15*(0.165702)+x16*(-0.0812625)+(0.0473355)); 
    h18 = tanh(x0*(-0.189908)+x1*(-0.276167)+x2*(0.310289)+x3*(-0.0801406)+x4*(-0.161663)+x5*(-0.185241)+x6*(0.318866)+x7*(0.252316)+x8*(-0.268467)+x9*(0.208533)+x10*(0.126277)+x11*(0.225369)+x12*(-0.229874)+x13*(-0.151619)+x14*(-0.339257)+x15*(0.379596)+x16*(0.345625)+(0.0578181)); 
    h19 = tanh(x0*(-0.174568)+x1*(0.195192)+x2*(-0.0408187)+x3*(0.024912)+x4*(-0.0195686)+x5*(0.17738)+x6*(0.19894)+x7*(0.0629898)+x8*(0.267093)+x9*(-0.162023)+x10*(-0.120356)+x11*(0.0364901)+x12*(-0.240444)+x13*(-0.218783)+x14*(-0.101108)+x15*(-0.016961)+x16*(-0.0833582)+(-0.0988862)); 
}

void FNN_13_2_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(-0.401916)+x1*(0.0949807)+x2*(-0.396806)+x3*(-0.419099)+x4*(0.476878)+x5*(0.0698015)+x6*(0.277606)+x7*(0.016184)+x8*(0.302553)+x9*(-0.140939)+x10*(-0.207031)+x11*(-0.0839006)+x12*(0.393632)+x13*(0.432126)+x14*(-0.0503233)+x15*(-0.100853)+x16*(-0.00595936)+x17*(-0.293063)+x18*(-0.482154)+x19*(0.382858)+(0.0351348)); 
    h1 = tanh(x0*(-0.252972)+x1*(0.557485)+x2*(0.111042)+x3*(-0.315544)+x4*(-0.244281)+x5*(0.0803795)+x6*(-0.122505)+x7*(-0.0187437)+x8*(-0.09274)+x9*(0.178204)+x10*(-0.402824)+x11*(-0.324986)+x12*(-0.0037657)+x13*(0.240809)+x14*(0.0492912)+x15*(0.147643)+x16*(0.370542)+x17*(-0.440098)+x18*(0.0727682)+x19*(0.22195)+(-0.103213)); 
    h2 = tanh(x0*(0.240524)+x1*(0.472729)+x2*(-0.55553)+x3*(-0.162003)+x4*(0.124354)+x5*(-0.282637)+x6*(-0.133641)+x7*(-0.54656)+x8*(0.156027)+x9*(-0.400133)+x10*(0.445623)+x11*(-0.254441)+x12*(-0.0193436)+x13*(-0.187459)+x14*(-0.136116)+x15*(0.0690721)+x16*(0.203517)+x17*(-0.408182)+x18*(-0.296979)+x19*(0.208592)+(-0.0567872)); 
    h3 = tanh(x0*(0.174875)+x1*(0.244782)+x2*(0.152196)+x3*(0.203112)+x4*(0.114275)+x5*(-0.053983)+x6*(-0.079633)+x7*(0.187143)+x8*(-0.0532573)+x9*(-0.257211)+x10*(-0.301224)+x11*(0.0403307)+x12*(0.227183)+x13*(-0.498566)+x14*(-0.152816)+x15*(0.11316)+x16*(0.114323)+x17*(-0.0924088)+x18*(0.0675565)+x19*(0.11587)+(-0.0287681)); 
    h4 = tanh(x0*(0.254112)+x1*(-0.142075)+x2*(-0.583216)+x3*(-0.227318)+x4*(0.282897)+x5*(-0.527694)+x6*(0.157763)+x7*(-0.623832)+x8*(0.245495)+x9*(-0.443892)+x10*(0.410128)+x11*(0.286136)+x12*(0.396509)+x13*(-0.308562)+x14*(0.208656)+x15*(-0.437867)+x16*(0.105059)+x17*(0.0341734)+x18*(-0.420979)+x19*(0.0754598)+(-0.0857625)); 
    h5 = tanh(x0*(0.00203836)+x1*(0.526062)+x2*(-0.0129682)+x3*(0.18869)+x4*(0.368067)+x5*(0.2763)+x6*(-0.0787367)+x7*(0.303032)+x8*(0.443412)+x9*(-0.225433)+x10*(0.156371)+x11*(-0.575817)+x12*(0.268563)+x13*(-0.0872943)+x14*(0.264957)+x15*(0.284903)+x16*(-0.293597)+x17*(-0.251317)+x18*(-0.314708)+x19*(-0.176399)+(-0.0179925)); 
    h6 = tanh(x0*(-0.242663)+x1*(-0.481555)+x2*(0.0455676)+x3*(0.53451)+x4*(-0.00653177)+x5*(-0.292655)+x6*(-0.0726646)+x7*(0.171183)+x8*(-0.139554)+x9*(-0.0416523)+x10*(0.244)+x11*(-0.224037)+x12*(-0.452285)+x13*(-0.299654)+x14*(-0.243855)+x15*(-0.264576)+x16*(-0.185905)+x17*(-0.052624)+x18*(0.175512)+x19*(-0.353628)+(-0.0905275)); 
    h7 = tanh(x0*(0.486177)+x1*(-0.350035)+x2*(-0.120154)+x3*(-0.096078)+x4*(-0.141007)+x5*(0.286113)+x6*(-0.271104)+x7*(0.264964)+x8*(0.232647)+x9*(0.233725)+x10*(-0.190628)+x11*(-0.00502257)+x12*(0.184879)+x13*(0.171775)+x14*(0.0851017)+x15*(-0.146649)+x16*(0.091357)+x17*(-0.397669)+x18*(-0.126153)+x19*(0.337155)+(0.0178054)); 
    h8 = tanh(x0*(-0.323408)+x1*(0.219242)+x2*(0.466911)+x3*(0.248254)+x4*(0.287539)+x5*(0.364666)+x6*(0.424286)+x7*(-0.346702)+x8*(-0.091142)+x9*(0.174065)+x10*(-0.0497089)+x11*(0.195502)+x12*(-0.0119067)+x13*(0.188159)+x14*(0.0990507)+x15*(0.116612)+x16*(-0.230046)+x17*(0.36242)+x18*(-0.0285013)+x19*(-0.26555)+(-0.0200359)); 
    h9 = tanh(x0*(-0.0337485)+x1*(-0.373338)+x2*(-0.0295811)+x3*(0.187908)+x4*(0.0350792)+x5*(0.509267)+x6*(0.0946327)+x7*(0.342537)+x8*(-0.53079)+x9*(0.710478)+x10*(-0.241378)+x11*(0.120275)+x12*(-0.531812)+x13*(0.0186182)+x14*(-0.435011)+x15*(-0.507667)+x16*(-0.488964)+x17*(-0.0198527)+x18*(-0.125952)+x19*(0.0155999)+(0.0130676)); 
}

void FNN_13_2_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(0.139348)+x1*(-0.224628)+x2*(-0.0935173)+x3*(0.620308)+x4*(-0.586653)+x5*(0.415768)+x6*(-0.0507853)+x7*(-0.552156)+x8*(0.568598)+x9*(-0.454913)+(0.0756584); 
    h1 = x0*(0.373072)+x1*(0.293165)+x2*(0.103043)+x3*(0.0857271)+x4*(-0.57837)+x5*(-0.627742)+x6*(0.508918)+x7*(0.587355)+x8*(0.560074)+x9*(-0.569384)+(0.0305981); 
    h2 = x0*(0.0260518)+x1*(-0.545309)+x2*(-0.179631)+x3*(-0.235638)+x4*(-0.497286)+x5*(-0.356729)+x6*(-0.29972)+x7*(0.0705558)+x8*(-0.360623)+x9*(-0.54233)+(0.0474054); 
}

void FNN_13_2(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_13_2_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_13_2_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_13_2_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_2(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_13_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void ModelKDTree_0(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    if ( x7 < 0.4 ) {
        ModelKDTree_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
    }
    else if ( x7 < 0.6 ) {
        float left_h0;
        float left_h1;
        float left_h2;
        float right_h0;
        float right_h1;
        float right_h2;
        
        ModelKDTree_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,left_h0,left_h1,left_h2);
        ModelKDTree_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,right_h0,right_h1,right_h2);
        
        float t = 0.5 + ( x7 - 0.5 ) / 0.2;
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
    x0 = (x0 - (0.106384)) / (0.893616);
    x1 = (x1 - (0.106384)) / (0.893616);
    x2 = (x2 - (0.106384)) / (0.893616);
    x3 = (x3 - (-0.999512)) / (1.99902);
    x4 = (x4 - (1.49902)) / (0.000977);
    x5 = (x5 - (-0.999512)) / (1.99902);
    x6 = (x6 - (-0.970703)) / (1.96973);
    x7 = (x7 - (-0.999512)) / (0.967194);
    x8 = (x8 - (-0.999023)) / (1.98486);
    x9 = (x9 - (0)) / (1);
    x10 = (x10 - (-1)) / (1);
    x11 = (x11 - (0)) / (1);
    x12 = (x12 - (1)) / (1);
    x13 = (x13 - (1)) / (1);
    x14 = (x14 - (1)) / (1);
    x15 = (x15 - (20)) / (1);
    x16 = (x16 - (1)) / (1);

    ModelKDTree_0(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);

    // map back
    h0 = h0 * (0.893616) + (0.106384);
    h1 = h1 * (0.893616) + (0.106384);
    h2 = h2 * (0.893616) + (0.106384);
}

