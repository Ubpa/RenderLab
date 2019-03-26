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

void FNN_12_0_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(0.23115)+x1*(0.0552462)+x2*(0.097961)+x3*(0.712428)+x4*(0.0703091)+x5*(0.0345065)+x6*(-0.26844)+x7*(0.223228)+x8*(0.36229)+x9*(-0.384405)+x10*(0.314693)+x11*(0.0497635)+x12*(0.157412)+x13*(-0.0407363)+x14*(-0.0331539)+x15*(0.100749)+x16*(-0.283771)+(-0.00901803)); 
    h1 = tanh(x0*(-0.198163)+x1*(0.25174)+x2*(0.116368)+x3*(-0.440033)+x4*(0.334245)+x5*(0.197493)+x6*(-0.251872)+x7*(0.0718918)+x8*(0.0592939)+x9*(0.326891)+x10*(-0.082559)+x11*(-0.139342)+x12*(-0.0908134)+x13*(0.364343)+x14*(0.217739)+x15*(-0.0166638)+x16*(0.0640252)+(-0.0876571)); 
    h2 = tanh(x0*(-0.223799)+x1*(-0.110366)+x2*(-0.289326)+x3*(-0.298979)+x4*(0.09805)+x5*(0.219006)+x6*(-0.0761002)+x7*(0.307684)+x8*(0.0154841)+x9*(0.180914)+x10*(-0.38696)+x11*(-0.268487)+x12*(-0.349051)+x13*(0.219224)+x14*(0.195898)+x15*(-0.0788381)+x16*(0.0692755)+(0.129638)); 
    h3 = tanh(x0*(0.0701836)+x1*(0.328168)+x2*(-0.118791)+x3*(-0.213695)+x4*(0.065544)+x5*(-0.290572)+x6*(-0.31788)+x7*(-0.151204)+x8*(0.240334)+x9*(-0.259426)+x10*(0.354192)+x11*(-0.0277492)+x12*(0.124092)+x13*(0.131565)+x14*(-0.0892269)+x15*(0.324225)+x16*(-0.0620551)+(-0.0187264)); 
    h4 = tanh(x0*(0.280928)+x1*(0.0258668)+x2*(-0.310884)+x3*(-0.0464097)+x4*(0.131296)+x5*(-0.28003)+x6*(-0.238589)+x7*(0.207736)+x8*(-0.00870273)+x9*(0.184311)+x10*(-0.0488708)+x11*(-0.0219002)+x12*(-0.336589)+x13*(0.311659)+x14*(0.28024)+x15*(-0.0945604)+x16*(0.023013)+(-0.0507192)); 
    h5 = tanh(x0*(0.163356)+x1*(-0.065058)+x2*(0.339092)+x3*(-0.0498741)+x4*(-0.100026)+x5*(-0.142853)+x6*(-0.124163)+x7*(-0.305098)+x8*(0.176768)+x9*(0.367302)+x10*(-0.247197)+x11*(-0.0238667)+x12*(-0.328409)+x13*(-0.287738)+x14*(-0.115613)+x15*(0.0232741)+x16*(0.194472)+(-0.0895624)); 
    h6 = tanh(x0*(0.308345)+x1*(0.182924)+x2*(0.367853)+x3*(-0.323471)+x4*(-0.111568)+x5*(0.08301)+x6*(-0.270153)+x7*(-0.205786)+x8*(0.25601)+x9*(0.346794)+x10*(-0.0929319)+x11*(-0.399877)+x12*(0.303588)+x13*(-0.264012)+x14*(0.361971)+x15*(0.117443)+x16*(-0.367552)+(0.016214)); 
    h7 = tanh(x0*(-0.219638)+x1*(-0.0464804)+x2*(-0.198741)+x3*(0.680948)+x4*(0.220039)+x5*(-0.156405)+x6*(0.0240435)+x7*(-0.308595)+x8*(0.371035)+x9*(-0.183359)+x10*(0.29877)+x11*(-0.35558)+x12*(0.0150039)+x13*(0.0844887)+x14*(-0.298773)+x15*(0.0265792)+x16*(-0.295431)+(-0.0071548)); 
    h8 = tanh(x0*(-0.234765)+x1*(0.228381)+x2*(0.380134)+x3*(0.451744)+x4*(-0.255527)+x5*(-0.196278)+x6*(-0.379127)+x7*(0.0733149)+x8*(-0.1517)+x9*(0.218087)+x10*(-0.259697)+x11*(0.0362418)+x12*(-0.273825)+x13*(0.107367)+x14*(0.176395)+x15*(0.226191)+x16*(0.0857451)+(0.105626)); 
    h9 = tanh(x0*(0.313045)+x1*(-0.40932)+x2*(0.108924)+x3*(-0.161837)+x4*(0.0162788)+x5*(-0.0936131)+x6*(-0.106066)+x7*(0.176455)+x8*(0.381109)+x9*(-0.234922)+x10*(-0.371523)+x11*(-0.294116)+x12*(0.34623)+x13*(0.247186)+x14*(-0.162126)+x15*(-0.181146)+x16*(0.0960559)+(-0.0381794)); 
    h10 = tanh(x0*(-0.286282)+x1*(0.398725)+x2*(0.00834691)+x3*(0.288468)+x4*(-0.326373)+x5*(0.393645)+x6*(-0.105043)+x7*(0.0876641)+x8*(0.0624119)+x9*(0.207188)+x10*(-0.303901)+x11*(-0.374067)+x12*(-0.315344)+x13*(0.255222)+x14*(-0.169722)+x15*(-0.121901)+x16*(0.245475)+(-0.0368867)); 
    h11 = tanh(x0*(0.0255453)+x1*(-0.359855)+x2*(-0.188542)+x3*(-0.0331373)+x4*(0.0152255)+x5*(0.291214)+x6*(0.141771)+x7*(-0.218462)+x8*(0.0396689)+x9*(0.371177)+x10*(0.0256984)+x11*(0.372072)+x12*(-0.0677124)+x13*(0.379722)+x14*(-0.293442)+x15*(0.154313)+x16*(0.262444)+(0.024723)); 
    h12 = tanh(x0*(0.0772707)+x1*(0.0891298)+x2*(0.243183)+x3*(0.227903)+x4*(-0.223691)+x5*(0.000410977)+x6*(0.0489047)+x7*(0.244147)+x8*(-0.242353)+x9*(-0.262548)+x10*(0.207312)+x11*(0.0133617)+x12*(0.25228)+x13*(0.191894)+x14*(-0.176265)+x15*(-0.00682822)+x16*(0.139095)+(-0.0283921)); 
    h13 = tanh(x0*(-0.0263401)+x1*(-0.239698)+x2*(-0.0287329)+x3*(0.133288)+x4*(-0.10296)+x5*(0.13265)+x6*(0.279578)+x7*(-0.135546)+x8*(-0.00964268)+x9*(0.172308)+x10*(0.396488)+x11*(-0.202238)+x12*(0.218189)+x13*(-0.356398)+x14*(-0.0290804)+x15*(0.292239)+x16*(0.370113)+(-0.0274885)); 
    h14 = tanh(x0*(-0.0690725)+x1*(0.0571634)+x2*(0.0342453)+x3*(0.360528)+x4*(0.365509)+x5*(-0.228648)+x6*(-0.168723)+x7*(-0.201822)+x8*(0.213149)+x9*(-0.29441)+x10*(0.362984)+x11*(-0.33034)+x12*(0.346562)+x13*(-0.228375)+x14*(0.0993211)+x15*(-0.0680366)+x16*(-0.395709)+(-0.0268086)); 
    h15 = tanh(x0*(-0.199887)+x1*(0.224074)+x2*(-0.0705391)+x3*(-0.189635)+x4*(-0.318039)+x5*(-0.0159852)+x6*(-0.138674)+x7*(-0.402104)+x8*(-0.0754443)+x9*(-0.314829)+x10*(-0.084673)+x11*(0.161968)+x12*(-0.319218)+x13*(-0.179166)+x14*(-0.0503147)+x15*(-0.191002)+x16*(0.244929)+(-0.107848)); 
    h16 = tanh(x0*(-0.0913099)+x1*(-0.0771277)+x2*(-0.0339979)+x3*(-0.0313881)+x4*(-0.0213096)+x5*(0.329008)+x6*(0.440241)+x7*(0.0324179)+x8*(-0.0875961)+x9*(-0.231466)+x10*(0.297984)+x11*(0.0952734)+x12*(-0.298645)+x13*(-0.303846)+x14*(0.02326)+x15*(-0.305063)+x16*(-0.0142902)+(-0.0641403)); 
    h17 = tanh(x0*(0.17409)+x1*(-0.278278)+x2*(0.157521)+x3*(-0.262092)+x4*(-0.455023)+x5*(0.0737828)+x6*(-0.390903)+x7*(0.175705)+x8*(0.129955)+x9*(0.0025672)+x10*(0.0688682)+x11*(-0.169242)+x12*(-0.191949)+x13*(0.221105)+x14*(-0.387076)+x15*(0.0713372)+x16*(-0.109728)+(0.0543116)); 
    h18 = tanh(x0*(0.218388)+x1*(0.0510904)+x2*(-0.408607)+x3*(0.110779)+x4*(0.236137)+x5*(0.302889)+x6*(0.0479695)+x7*(-0.291815)+x8*(-0.26231)+x9*(-0.06795)+x10*(-0.225096)+x11*(-0.0119086)+x12*(0.0513958)+x13*(0.0375197)+x14*(-0.227516)+x15*(-0.25154)+x16*(-0.312622)+(0.105664)); 
    h19 = tanh(x0*(0.135424)+x1*(0.102133)+x2*(-0.159516)+x3*(-0.00647391)+x4*(0.304886)+x5*(-0.15146)+x6*(-0.118267)+x7*(-0.276691)+x8*(-0.40702)+x9*(-0.388981)+x10*(-0.0984674)+x11*(0.35821)+x12*(-0.13738)+x13*(0.299145)+x14*(0.101475)+x15*(0.0263847)+x16*(-0.205488)+(0.00871687)); 
}

void FNN_12_0_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(-0.0921093)+x1*(0.455671)+x2*(-0.340184)+x3*(0.0261235)+x4*(-0.0176729)+x5*(0.1039)+x6*(-0.236102)+x7*(0.0972736)+x8*(-0.151394)+x9*(-0.247951)+x10*(-0.155053)+x11*(0.331614)+x12*(0.14249)+x13*(-0.291084)+x14*(-0.369942)+x15*(0.284948)+x16*(0.0563543)+x17*(0.330548)+x18*(-0.0618228)+x19*(-0.242899)+(-0.00114397)); 
    h1 = tanh(x0*(0.119374)+x1*(-0.215703)+x2*(-0.337251)+x3*(0.396328)+x4*(-0.343474)+x5*(-0.1441)+x6*(-0.061987)+x7*(0.159661)+x8*(-0.190326)+x9*(-0.417897)+x10*(0.173083)+x11*(0.235273)+x12*(-0.0312672)+x13*(0.0489165)+x14*(-0.0484383)+x15*(-0.02732)+x16*(-0.220756)+x17*(-0.00390743)+x18*(-0.0822676)+x19*(0.0318427)+(-0.00635603)); 
    h2 = tanh(x0*(0.181272)+x1*(0.440339)+x2*(0.279372)+x3*(-0.412219)+x4*(0.369323)+x5*(0.106636)+x6*(0.274734)+x7*(-0.0635929)+x8*(0.0530664)+x9*(-0.106066)+x10*(0.253845)+x11*(0.391376)+x12*(0.205496)+x13*(0.0850934)+x14*(0.153649)+x15*(0.210543)+x16*(0.389039)+x17*(-0.204802)+x18*(0.370172)+x19*(-0.0598583)+(-0.0626177)); 
    h3 = tanh(x0*(-0.182446)+x1*(-0.156394)+x2*(-0.13225)+x3*(-0.118855)+x4*(-0.207354)+x5*(-0.280964)+x6*(0.004208)+x7*(-0.0931913)+x8*(0.0245327)+x9*(0.203447)+x10*(-0.176808)+x11*(0.0544953)+x12*(-0.34424)+x13*(-0.0368967)+x14*(-0.123009)+x15*(-0.461617)+x16*(-0.23583)+x17*(0.253905)+x18*(-0.257895)+x19*(-0.120709)+(0.00453436)); 
    h4 = tanh(x0*(-0.0869435)+x1*(-0.145495)+x2*(-0.0659413)+x3*(0.201397)+x4*(0.0510988)+x5*(0.00218438)+x6*(0.157451)+x7*(0.161284)+x8*(-0.185348)+x9*(-0.0209346)+x10*(0.256071)+x11*(0.177235)+x12*(-0.236998)+x13*(0.111901)+x14*(-0.286998)+x15*(-0.269506)+x16*(0.0882878)+x17*(0.0984152)+x18*(0.200377)+x19*(0.23407)+(0.0163081)); 
    h5 = tanh(x0*(0.564582)+x1*(-0.468736)+x2*(0.113221)+x3*(-0.221126)+x4*(-0.382914)+x5*(-0.20349)+x6*(-0.27566)+x7*(0.510294)+x8*(0.402534)+x9*(-0.367249)+x10*(0.125448)+x11*(0.326539)+x12*(0.0767698)+x13*(-0.196009)+x14*(0.226849)+x15*(-0.388949)+x16*(-0.390759)+x17*(0.0964199)+x18*(0.257802)+x19*(0.151262)+(0.112138)); 
    h6 = tanh(x0*(0.0938424)+x1*(0.45245)+x2*(0.171226)+x3*(-0.260655)+x4*(-0.117702)+x5*(-0.246309)+x6*(0.0438604)+x7*(0.097204)+x8*(-0.03152)+x9*(-0.362874)+x10*(-0.427112)+x11*(-0.454131)+x12*(-0.306045)+x13*(-0.321978)+x14*(-0.0387079)+x15*(0.177887)+x16*(0.212327)+x17*(0.151384)+x18*(0.00345208)+x19*(-0.0424097)+(0.0408768)); 
    h7 = tanh(x0*(0.304605)+x1*(0.0980265)+x2*(-0.218456)+x3*(0.250889)+x4*(0.0558672)+x5*(-0.510145)+x6*(0.224688)+x7*(0.415299)+x8*(-0.217322)+x9*(-0.243399)+x10*(-0.131269)+x11*(-0.0332479)+x12*(-0.180575)+x13*(-0.16453)+x14*(-0.242455)+x15*(-0.0245168)+x16*(0.182128)+x17*(0.489056)+x18*(0.0390897)+x19*(0.338861)+(-0.0496088)); 
    h8 = tanh(x0*(0.0167815)+x1*(-0.109858)+x2*(-0.418976)+x3*(-0.0800778)+x4*(0.382091)+x5*(0.392366)+x6*(-0.40347)+x7*(0.298947)+x8*(0.226677)+x9*(-0.00617267)+x10*(0.395327)+x11*(-0.226951)+x12*(-0.0125191)+x13*(0.146909)+x14*(0.120797)+x15*(0.389639)+x16*(0.0873755)+x17*(-0.155135)+x18*(-0.29205)+x19*(-0.377613)+(-0.0668379)); 
    h9 = tanh(x0*(0.19533)+x1*(0.115601)+x2*(0.487861)+x3*(0.236443)+x4*(0.430014)+x5*(0.0102478)+x6*(0.139853)+x7*(0.385461)+x8*(-0.102607)+x9*(0.163614)+x10*(0.0718396)+x11*(0.0980117)+x12*(-0.073912)+x13*(-0.318993)+x14*(0.293078)+x15*(-0.269498)+x16*(-0.0921521)+x17*(-0.505925)+x18*(0.270095)+x19*(-0.35117)+(-0.0306384)); 
}

void FNN_12_0_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(0.558626)+x1*(0.117171)+x2*(-0.220209)+x3*(-0.139368)+x4*(-0.282129)+x5*(0.392203)+x6*(-0.101071)+x7*(-0.286395)+x8*(-0.372527)+x9*(0.105632)+(0.0083759); 
    h1 = x0*(-0.00779393)+x1*(0.232572)+x2*(-0.256973)+x3*(0.103769)+x4*(-0.192268)+x5*(0.572937)+x6*(0.11102)+x7*(0.0889995)+x8*(-0.516398)+x9*(-0.328598)+(0.0517056); 
    h2 = x0*(-0.13059)+x1*(-0.417114)+x2*(-0.20986)+x3*(-0.18777)+x4*(0.488799)+x5*(0.249377)+x6*(0.103243)+x7*(-0.0536807)+x8*(0.0142728)+x9*(-0.17596)+(0.0308666); 
}

void FNN_12_0(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_12_0_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_12_0_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_12_0_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_0(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_12_0(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void Model(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,    
out float h0,out float h1,out float h2)
{
    // map to unit hyperbox
    x0 = (x0 - (0.022278)) / (0.191345);
    x1 = (x1 - (0.066833)) / (0.57428);
    x2 = (x2 - (0.022278)) / (0.191345);
    x3 = (x3 - (-0.999512)) / (1.99902);
    x4 = (x4 - (4e-05)) / (1.49898);
    x5 = (x5 - (-1)) / (0.000488);
    x6 = (x6 - (-0.994141)) / (1.91455);
    x7 = (x7 - (-0.860352)) / (1.79053);
    x8 = (x8 - (0.083801)) / (0.915711);
    x9 = (x9 - (0)) / (1);
    x10 = (x10 - (0)) / (1);
    x11 = (x11 - (1)) / (1);
    x12 = (x12 - (0.199951)) / (1);
    x13 = (x13 - (0.599609)) / (1);
    x14 = (x14 - (0.199951)) / (1);
    x15 = (x15 - (20)) / (1);
    x16 = (x16 - (1)) / (1);

    ModelKDTree_0(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);

    // map back
    h0 = h0 * (0.191345) + (0.022278);
    h1 = h1 * (0.57428) + (0.066833);
    h2 = h2 * (0.191345) + (0.022278);
}

