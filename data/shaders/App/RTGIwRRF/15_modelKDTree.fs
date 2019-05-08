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

void FNN_15_1_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(0.200327)+x1*(-0.282252)+x2*(-0.0186779)+x3*(0.403409)+x4*(0.129622)+x5*(0.0781808)+x6*(0.36698)+x7*(-0.158543)+x8*(0.367631)+x9*(0.0072276)+x10*(0.068905)+x11*(-0.155011)+x12*(-0.250365)+x13*(-0.109436)+x14*(0.345333)+x15*(-0.349242)+x16*(-0.126574)+(0.00455752)); 
    h1 = tanh(x0*(0.134382)+x1*(0.171819)+x2*(-0.0535835)+x3*(0.31004)+x4*(-0.0563581)+x5*(-0.137167)+x6*(0.0211628)+x7*(-0.378805)+x8*(-0.123279)+x9*(-0.401915)+x10*(0.230954)+x11*(0.232295)+x12*(0.111363)+x13*(-0.117953)+x14*(0.297565)+x15*(0.22064)+x16*(0.386912)+(-0.0207053)); 
    h2 = tanh(x0*(-0.285249)+x1*(-0.500277)+x2*(-0.297943)+x3*(0.283695)+x4*(0.174849)+x5*(-0.463767)+x6*(-0.329246)+x7*(0.138214)+x8*(-0.173349)+x9*(-0.164342)+x10*(-0.280147)+x11*(0.0374063)+x12*(-0.178395)+x13*(0.130296)+x14*(-0.0661989)+x15*(0.252355)+x16*(-0.194041)+(-0.040569)); 
    h3 = tanh(x0*(0.0666373)+x1*(0.25597)+x2*(-0.263818)+x3*(-0.0411024)+x4*(0.1919)+x5*(0.327243)+x6*(-0.407152)+x7*(0.176374)+x8*(-0.14311)+x9*(-0.289204)+x10*(0.228243)+x11*(0.0829642)+x12*(-0.0949591)+x13*(-0.205726)+x14*(-0.143348)+x15*(-0.288506)+x16*(0.0774793)+(-0.0456262)); 
    h4 = tanh(x0*(0.429413)+x1*(0.321653)+x2*(-0.115943)+x3*(-0.222294)+x4*(0.082558)+x5*(-0.0984278)+x6*(0.255323)+x7*(0.0735202)+x8*(0.184011)+x9*(-0.108413)+x10*(0.266421)+x11*(0.0207002)+x12*(0.353798)+x13*(-0.128847)+x14*(0.167986)+x15*(-0.317141)+x16*(-0.259346)+(-5.69719e-05)); 
    h5 = tanh(x0*(-0.303743)+x1*(-0.266574)+x2*(0.212206)+x3*(0.242793)+x4*(0.378895)+x5*(0.00791624)+x6*(0.368842)+x7*(-0.377649)+x8*(0.185486)+x9*(-0.341447)+x10*(0.184421)+x11*(0.031957)+x12*(0.390373)+x13*(0.183327)+x14*(0.278124)+x15*(0.0578867)+x16*(0.343948)+(-0.0349714)); 
    h6 = tanh(x0*(-0.270313)+x1*(-0.114883)+x2*(0.304489)+x3*(-0.119382)+x4*(-0.294883)+x5*(0.250672)+x6*(-0.0447954)+x7*(0.271672)+x8*(-0.370527)+x9*(0.0115163)+x10*(-0.241503)+x11*(0.09856)+x12*(0.167593)+x13*(0.366021)+x14*(0.371285)+x15*(-0.132946)+x16*(-0.0798562)+(-0.015968)); 
    h7 = tanh(x0*(0.231351)+x1*(-0.379374)+x2*(0.384937)+x3*(-0.0583524)+x4*(-0.187004)+x5*(0.248015)+x6*(-0.0693713)+x7*(0.217338)+x8*(-0.272945)+x9*(-0.107002)+x10*(0.190111)+x11*(-0.352208)+x12*(-0.114481)+x13*(0.218747)+x14*(-0.317834)+x15*(0.0722978)+x16*(-0.0686226)+(-0.0263692)); 
    h8 = tanh(x0*(-0.0401295)+x1*(0.213105)+x2*(-0.272719)+x3*(0.271231)+x4*(-0.174902)+x5*(-0.0653662)+x6*(-0.00763237)+x7*(0.194453)+x8*(-0.0388746)+x9*(-0.399909)+x10*(-0.0359205)+x11*(0.125391)+x12*(-0.21664)+x13*(0.168409)+x14*(0.0367311)+x15*(-0.381527)+x16*(0.2883)+(0.0383277)); 
    h9 = tanh(x0*(-0.172568)+x1*(-0.314555)+x2*(0.199884)+x3*(-0.180171)+x4*(-0.0751199)+x5*(-0.138921)+x6*(-0.0470357)+x7*(-0.0621623)+x8*(0.0552202)+x9*(0.297176)+x10*(-0.323538)+x11*(-0.16292)+x12*(0.0688981)+x13*(0.228452)+x14*(0.141781)+x15*(0.171776)+x16*(0.396586)+(0.00961798)); 
    h10 = tanh(x0*(-0.140579)+x1*(0.101955)+x2*(-0.23545)+x3*(0.305766)+x4*(0.0846395)+x5*(0.343803)+x6*(-0.167723)+x7*(-0.00231538)+x8*(0.000547842)+x9*(-0.0366401)+x10*(-0.137063)+x11*(0.142351)+x12*(0.154022)+x13*(0.259345)+x14*(0.0355089)+x15*(-0.0509056)+x16*(-0.0558659)+(0.0158304)); 
    h11 = tanh(x0*(0.27138)+x1*(-0.156585)+x2*(0.297117)+x3*(-0.215354)+x4*(-0.321904)+x5*(0.459828)+x6*(0.201057)+x7*(-0.000661905)+x8*(-0.220463)+x9*(-0.342721)+x10*(-0.0391257)+x11*(-0.252193)+x12*(0.0863418)+x13*(-0.264648)+x14*(-0.329214)+x15*(-0.228034)+x16*(-0.349527)+(0.0302329)); 
    h12 = tanh(x0*(0.00179101)+x1*(-0.13118)+x2*(0.299069)+x3*(-0.0596933)+x4*(0.213348)+x5*(0.0451203)+x6*(0.351837)+x7*(0.278448)+x8*(0.103479)+x9*(-0.332365)+x10*(-0.0276571)+x11*(-0.0228337)+x12*(0.168448)+x13*(-0.13039)+x14*(-0.351398)+x15*(-0.269465)+x16*(0.256742)+(-0.0689455)); 
    h13 = tanh(x0*(-0.272333)+x1*(-0.17133)+x2*(0.219577)+x3*(-0.218739)+x4*(-0.0795286)+x5*(0.219648)+x6*(-0.404996)+x7*(0.0534872)+x8*(0.0216294)+x9*(-0.24387)+x10*(-0.113555)+x11*(0.159458)+x12*(0.401182)+x13*(0.356564)+x14*(-0.342587)+x15*(0.298953)+x16*(0.178186)+(-4.60316e-05)); 
    h14 = tanh(x0*(-0.0757122)+x1*(0.261915)+x2*(-0.247794)+x3*(0.246848)+x4*(0.179192)+x5*(0.342594)+x6*(0.149758)+x7*(0.220072)+x8*(0.0294555)+x9*(0.233357)+x10*(-0.0452994)+x11*(0.0339619)+x12*(0.239351)+x13*(0.0389007)+x14*(0.206795)+x15*(0.109311)+x16*(-0.119317)+(0.0298202)); 
    h15 = tanh(x0*(0.0459879)+x1*(-0.0168043)+x2*(-0.197401)+x3*(-0.450093)+x4*(0.101376)+x5*(0.334597)+x6*(0.173696)+x7*(0.291099)+x8*(-0.301257)+x9*(0.317806)+x10*(0.226384)+x11*(-0.214954)+x12*(-0.397676)+x13*(-0.385658)+x14*(0.319162)+x15*(0.319385)+x16*(-0.268688)+(-0.0398619)); 
    h16 = tanh(x0*(-0.0161751)+x1*(-0.188292)+x2*(-0.0195055)+x3*(0.108085)+x4*(0.00921559)+x5*(-0.383496)+x6*(0.207428)+x7*(0.243145)+x8*(0.160634)+x9*(-0.142987)+x10*(0.0692655)+x11*(0.165926)+x12*(0.374864)+x13*(0.253578)+x14*(-0.0421622)+x15*(0.00282106)+x16*(-0.202218)+(0.0543626)); 
    h17 = tanh(x0*(0.249996)+x1*(0.150435)+x2*(0.23414)+x3*(0.214432)+x4*(-0.235674)+x5*(0.159249)+x6*(0.0485552)+x7*(0.0920031)+x8*(-0.217962)+x9*(0.0879506)+x10*(0.0822294)+x11*(-0.228713)+x12*(0.391389)+x13*(-0.285361)+x14*(0.116635)+x15*(0.0571607)+x16*(-0.247544)+(-0.0639544)); 
    h18 = tanh(x0*(-0.236005)+x1*(-0.000526268)+x2*(0.110335)+x3*(0.296623)+x4*(0.188652)+x5*(-0.339687)+x6*(-0.141314)+x7*(0.316465)+x8*(-0.173914)+x9*(-0.275724)+x10*(-0.249117)+x11*(-0.0664202)+x12*(0.37839)+x13*(-0.162507)+x14*(-0.199971)+x15*(-0.0144064)+x16*(0.266315)+(-0.0493724)); 
    h19 = tanh(x0*(-0.328945)+x1*(-0.34363)+x2*(-0.27343)+x3*(0.0883464)+x4*(-0.0252366)+x5*(-0.568211)+x6*(-0.314983)+x7*(-0.283111)+x8*(0.220113)+x9*(0.280731)+x10*(-0.114761)+x11*(0.302166)+x12*(0.232389)+x13*(-0.334685)+x14*(0.388965)+x15*(0.336884)+x16*(0.335569)+(0.00893216)); 
}

void FNN_15_1_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(0.208772)+x1*(-0.298507)+x2*(0.0877131)+x3*(0.30939)+x4*(-0.388724)+x5*(0.397665)+x6*(0.0736591)+x7*(0.39039)+x8*(0.0134308)+x9*(-0.42234)+x10*(-0.305671)+x11*(-0.261253)+x12*(-0.299093)+x13*(0.353183)+x14*(-0.425448)+x15*(0.121251)+x16*(-0.268978)+x17*(-0.130359)+x18*(0.592433)+x19*(-0.406945)+(0.0097716)); 
    h1 = tanh(x0*(0.265538)+x1*(-0.189152)+x2*(-0.308381)+x3*(0.280665)+x4*(0.134745)+x5*(0.0985211)+x6*(-0.265999)+x7*(-0.24703)+x8*(0.259785)+x9*(-0.00945708)+x10*(-0.386186)+x11*(-0.0393144)+x12*(-0.251673)+x13*(0.15258)+x14*(0.468181)+x15*(-0.155173)+x16*(0.368123)+x17*(-0.25715)+x18*(-0.188308)+x19*(-0.462601)+(0.0300646)); 
    h2 = tanh(x0*(0.235502)+x1*(-0.129658)+x2*(-0.00503819)+x3*(0.470938)+x4*(-0.34224)+x5*(0.10531)+x6*(-0.399846)+x7*(0.390908)+x8*(0.0593273)+x9*(0.168243)+x10*(-0.446866)+x11*(-0.0615548)+x12*(-0.304291)+x13*(0.0439749)+x14*(0.298445)+x15*(0.276977)+x16*(0.0618896)+x17*(0.271215)+x18*(0.165599)+x19*(0.215492)+(-0.0188133)); 
    h3 = tanh(x0*(0.0807073)+x1*(-0.053543)+x2*(-0.34428)+x3*(0.0702991)+x4*(-0.00887673)+x5*(-0.113308)+x6*(-0.141339)+x7*(-0.0462531)+x8*(-0.0266596)+x9*(-0.312988)+x10*(-0.124176)+x11*(0.0840552)+x12*(-0.365039)+x13*(-0.114308)+x14*(0.149077)+x15*(-0.171083)+x16*(-0.385275)+x17*(-0.216455)+x18*(0.310374)+x19*(-0.180694)+(0.00318771)); 
    h4 = tanh(x0*(-0.0348396)+x1*(0.309614)+x2*(-0.279627)+x3*(0.242222)+x4*(-0.479458)+x5*(0.0465952)+x6*(0.241853)+x7*(0.399132)+x8*(-0.284609)+x9*(0.392436)+x10*(0.193916)+x11*(-0.335497)+x12*(-0.229969)+x13*(-0.192371)+x14*(-0.219051)+x15*(0.266515)+x16*(-0.112014)+x17*(-0.294955)+x18*(0.326439)+x19*(-0.213772)+(0.00775884)); 
    h5 = tanh(x0*(-0.306564)+x1*(-0.159331)+x2*(0.173906)+x3*(0.205154)+x4*(-0.453438)+x5*(-0.169259)+x6*(0.135376)+x7*(-0.291754)+x8*(0.0387691)+x9*(0.198611)+x10*(0.164302)+x11*(0.319682)+x12*(-0.351768)+x13*(0.103545)+x14*(0.370918)+x15*(0.234675)+x16*(0.246919)+x17*(0.078494)+x18*(-0.250177)+x19*(0.229006)+(0.0199179)); 
    h6 = tanh(x0*(-0.0432417)+x1*(0.158821)+x2*(0.0765883)+x3*(0.302457)+x4*(-0.396632)+x5*(0.0540541)+x6*(-0.139957)+x7*(0.460212)+x8*(0.0294507)+x9*(-0.104182)+x10*(-0.183071)+x11*(0.384103)+x12*(-0.295431)+x13*(0.328928)+x14*(-0.0791791)+x15*(-0.00581163)+x16*(-0.307626)+x17*(-0.327839)+x18*(0.3841)+x19*(0.255856)+(-0.0166454)); 
    h7 = tanh(x0*(0.167557)+x1*(-0.256947)+x2*(-0.132483)+x3*(0.168318)+x4*(-0.399263)+x5*(-0.372263)+x6*(0.122536)+x7*(-0.40154)+x8*(-0.208262)+x9*(-0.352039)+x10*(0.028654)+x11*(-0.070131)+x12*(0.189)+x13*(0.221686)+x14*(-0.405221)+x15*(0.131281)+x16*(0.0214337)+x17*(0.217921)+x18*(-0.178752)+x19*(-0.0902849)+(-0.0285141)); 
    h8 = tanh(x0*(-0.225522)+x1*(0.056138)+x2*(0.165696)+x3*(0.0542365)+x4*(0.274245)+x5*(-0.157857)+x6*(0.247661)+x7*(-0.56172)+x8*(0.315759)+x9*(0.314069)+x10*(-0.188295)+x11*(0.00488033)+x12*(-0.276431)+x13*(-0.197028)+x14*(-0.148674)+x15*(0.412147)+x16*(-0.182833)+x17*(-0.216696)+x18*(-0.416282)+x19*(0.434176)+(0.00509012)); 
    h9 = tanh(x0*(0.308642)+x1*(-0.0252352)+x2*(-0.232489)+x3*(-0.165361)+x4*(0.00157184)+x5*(-0.041896)+x6*(-0.417873)+x7*(0.318323)+x8*(-0.0182339)+x9*(-0.138796)+x10*(0.269088)+x11*(-0.355684)+x12*(-0.448207)+x13*(0.237751)+x14*(-0.165372)+x15*(0.0849049)+x16*(0.167021)+x17*(-0.213003)+x18*(0.135682)+x19*(-0.236509)+(0.0481503)); 
}

void FNN_15_1_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(0.0450609)+x1*(0.647843)+x2*(-0.433563)+x3*(-0.0597917)+x4*(0.311342)+x5*(0.572968)+x6*(0.268966)+x7*(0.156092)+x8*(-0.282098)+x9*(0.438007)+(0.0569567); 
    h1 = x0*(0.375365)+x1*(0.488887)+x2*(-0.260473)+x3*(0.392462)+x4*(-0.379513)+x5*(0.578992)+x6*(0.194651)+x7*(-0.401161)+x8*(0.177687)+x9*(-0.0655368)+(0.0324495); 
    h2 = x0*(0.0526534)+x1*(0.566917)+x2*(-0.321706)+x3*(-0.227423)+x4*(0.105851)+x5*(0.151897)+x6*(0.46685)+x7*(0.0346298)+x8*(-0.204866)+x9*(0.0498933)+(-0.0135521); 
}

void FNN_15_1(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_15_1_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_15_1_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_15_1_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_1(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_15_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void FNN_15_2_Layer_0_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9,out float h10,out float h11,out float h12,out float h13,out float h14,out float h15,out float h16,out float h17,out float h18,out float h19)
{
    h0 = tanh(x0*(-0.0123379)+x1*(0.302353)+x2*(-0.0796532)+x3*(-0.184379)+x4*(-0.0246)+x5*(0.223424)+x6*(0.0224702)+x7*(0.416751)+x8*(-0.325562)+x9*(-0.20917)+x10*(0.0873346)+x11*(-0.067387)+x12*(-0.290161)+x13*(-0.331063)+x14*(0.125092)+x15*(-0.341887)+x16*(0.228969)+(-0.0440217)); 
    h1 = tanh(x0*(-0.270764)+x1*(-0.0482336)+x2*(0.167778)+x3*(0.179715)+x4*(0.0709487)+x5*(0.310693)+x6*(0.0901543)+x7*(0.186591)+x8*(-0.070565)+x9*(-0.366565)+x10*(-0.133664)+x11*(-0.321982)+x12*(0.0550192)+x13*(0.386519)+x14*(-0.356311)+x15*(-0.272844)+x16*(-0.0394808)+(-0.063042)); 
    h2 = tanh(x0*(-0.0237667)+x1*(0.368681)+x2*(-0.130584)+x3*(0.130925)+x4*(-0.266496)+x5*(-0.0466565)+x6*(0.0586778)+x7*(0.0268362)+x8*(0.325903)+x9*(-0.332765)+x10*(0.159728)+x11*(-0.0587581)+x12*(-0.0284066)+x13*(0.0336376)+x14*(-0.260184)+x15*(0.245844)+x16*(0.162977)+(0.0664647)); 
    h3 = tanh(x0*(-0.36697)+x1*(0.100153)+x2*(-0.14781)+x3*(-0.27557)+x4*(-0.175015)+x5*(-0.440543)+x6*(-0.039451)+x7*(-0.215788)+x8*(0.114033)+x9*(0.226418)+x10*(-0.0861758)+x11*(-0.101744)+x12*(-0.139223)+x13*(0.196569)+x14*(0.358987)+x15*(0.39848)+x16*(0.0856918)+(-0.0626568)); 
    h4 = tanh(x0*(-0.00212239)+x1*(0.27642)+x2*(-0.34606)+x3*(-0.0395672)+x4*(-0.309775)+x5*(0.0620353)+x6*(0.0355661)+x7*(-0.225671)+x8*(0.335119)+x9*(0.288442)+x10*(-0.116049)+x11*(0.369805)+x12*(0.359647)+x13*(-0.309487)+x14*(-0.10818)+x15*(-0.101537)+x16*(0.375015)+(0.0554585)); 
    h5 = tanh(x0*(0.278392)+x1*(-0.146902)+x2*(0.269136)+x3*(0.0141872)+x4*(0.00208695)+x5*(-0.305267)+x6*(-0.35191)+x7*(0.0822175)+x8*(-0.169123)+x9*(-0.00528485)+x10*(-0.278764)+x11*(0.353932)+x12*(-0.369175)+x13*(-0.357292)+x14*(-0.0404206)+x15*(0.287817)+x16*(0.329737)+(0.0132746)); 
    h6 = tanh(x0*(0.205167)+x1*(0.241872)+x2*(0.175993)+x3*(-0.0612026)+x4*(-0.159907)+x5*(-0.297818)+x6*(-0.347027)+x7*(0.0551379)+x8*(0.139204)+x9*(0.251442)+x10*(-0.341293)+x11*(-0.102868)+x12*(0.119143)+x13*(-0.226154)+x14*(-0.344152)+x15*(-0.256641)+x16*(0.340236)+(0.0122944)); 
    h7 = tanh(x0*(0.135584)+x1*(-0.151145)+x2*(-0.213802)+x3*(0.0957891)+x4*(0.272248)+x5*(-0.10356)+x6*(-0.236251)+x7*(0.251887)+x8*(0.0589908)+x9*(-0.147118)+x10*(0.122249)+x11*(0.40015)+x12*(0.374423)+x13*(0.256797)+x14*(-0.195071)+x15*(0.187024)+x16*(0.0991616)+(-0.010178)); 
    h8 = tanh(x0*(-0.206461)+x1*(0.489477)+x2*(0.366092)+x3*(-0.282153)+x4*(0.342388)+x5*(-0.347215)+x6*(0.350884)+x7*(0.302599)+x8*(-0.117652)+x9*(0.2713)+x10*(0.208854)+x11*(0.366982)+x12*(0.280426)+x13*(-0.0392033)+x14*(-0.236568)+x15*(0.180721)+x16*(0.398835)+(-0.0566367)); 
    h9 = tanh(x0*(0.155629)+x1*(-0.367571)+x2*(-0.284419)+x3*(-0.267666)+x4*(-0.10905)+x5*(-0.0495774)+x6*(-0.107909)+x7*(0.227198)+x8*(0.287248)+x9*(0.244141)+x10*(0.00660998)+x11*(-0.277174)+x12*(-0.350611)+x13*(-0.227609)+x14*(-0.124616)+x15*(0.333385)+x16*(0.14732)+(-0.0552349)); 
    h10 = tanh(x0*(-0.182434)+x1*(-0.210077)+x2*(0.0109673)+x3*(0.329074)+x4*(0.0655445)+x5*(0.129616)+x6*(-0.339405)+x7*(-0.166396)+x8*(-0.00599523)+x9*(-0.136101)+x10*(0.0234723)+x11*(0.0094074)+x12*(0.165071)+x13*(0.322605)+x14*(0.379928)+x15*(-0.0455261)+x16*(0.213387)+(0.0402941)); 
    h11 = tanh(x0*(-0.0456927)+x1*(0.0705012)+x2*(-0.524518)+x3*(-0.288686)+x4*(0.25124)+x5*(0.0360876)+x6*(0.0105261)+x7*(0.343516)+x8*(0.220947)+x9*(0.321528)+x10*(0.0216157)+x11*(0.0470428)+x12*(-0.348514)+x13*(-0.198934)+x14*(-0.19219)+x15*(-0.387923)+x16*(0.21854)+(0.078502)); 
    h12 = tanh(x0*(-0.291781)+x1*(-0.172538)+x2*(-0.233724)+x3*(0.299734)+x4*(-0.0677691)+x5*(-0.102156)+x6*(-0.0577141)+x7*(-0.14982)+x8*(0.0482423)+x9*(-0.389184)+x10*(-0.131809)+x11*(0.0767281)+x12*(0.347456)+x13*(0.320652)+x14*(0.337232)+x15*(0.271585)+x16*(-0.162657)+(-0.031097)); 
    h13 = tanh(x0*(-0.442091)+x1*(0.140695)+x2*(0.0777584)+x3*(-0.137237)+x4*(0.0150269)+x5*(0.221038)+x6*(0.154371)+x7*(-0.117285)+x8*(-0.274165)+x9*(0.386506)+x10*(0.251636)+x11*(-0.0455647)+x12*(0.142264)+x13*(0.320036)+x14*(-0.218012)+x15*(0.134962)+x16*(-0.0831411)+(0.0237853)); 
    h14 = tanh(x0*(-0.0209804)+x1*(0.302148)+x2*(-0.0573508)+x3*(0.0556793)+x4*(0.333276)+x5*(0.0812587)+x6*(-0.152711)+x7*(0.256095)+x8*(0.25621)+x9*(0.201913)+x10*(0.121042)+x11*(0.367954)+x12*(-0.400003)+x13*(-0.0481918)+x14*(0.23105)+x15*(0.320948)+x16*(-0.194637)+(0.0120948)); 
    h15 = tanh(x0*(-0.260047)+x1*(0.123547)+x2*(-0.000523242)+x3*(-0.321396)+x4*(-0.0252127)+x5*(0.30851)+x6*(0.0824769)+x7*(0.291292)+x8*(-0.0151693)+x9*(-0.228378)+x10*(-0.24666)+x11*(0.20926)+x12*(0.205505)+x13*(0.119274)+x14*(-0.233436)+x15*(0.26908)+x16*(0.296488)+(0.0354451)); 
    h16 = tanh(x0*(0.0025825)+x1*(0.0246409)+x2*(0.343758)+x3*(-0.227558)+x4*(-0.148116)+x5*(0.321285)+x6*(0.181279)+x7*(0.195398)+x8*(-0.291332)+x9*(0.331652)+x10*(-0.307258)+x11*(-0.382803)+x12*(-0.150894)+x13*(0.354891)+x14*(-0.175416)+x15*(-0.348613)+x16*(-0.326362)+(0.0623043)); 
    h17 = tanh(x0*(0.400075)+x1*(-0.107053)+x2*(0.0482643)+x3*(-0.0737074)+x4*(0.119176)+x5*(-0.0806474)+x6*(-0.261452)+x7*(0.246184)+x8*(0.067078)+x9*(-0.0565038)+x10*(-0.164669)+x11*(-0.224186)+x12*(0.120472)+x13*(0.171713)+x14*(0.319007)+x15*(0.344491)+x16*(0.0296056)+(-0.0161246)); 
    h18 = tanh(x0*(0.219509)+x1*(0.0217605)+x2*(-0.0721654)+x3*(0.142362)+x4*(-0.0722252)+x5*(0.148969)+x6*(0.1265)+x7*(-0.0537909)+x8*(-0.281975)+x9*(-0.138857)+x10*(0.125159)+x11*(-0.105063)+x12*(-0.235659)+x13*(0.0884314)+x14*(-0.400281)+x15*(-0.146732)+x16*(0.25944)+(0.0123381)); 
    h19 = tanh(x0*(0.30116)+x1*(0.295615)+x2*(0.0192551)+x3*(0.265756)+x4*(-0.106255)+x5*(-0.183083)+x6*(0.339455)+x7*(-0.0880078)+x8*(0.212797)+x9*(0.130633)+x10*(0.170028)+x11*(-0.137231)+x12*(-0.263208)+x13*(-0.16842)+x14*(-0.175571)+x15*(-0.35847)+x16*(0.361282)+(-0.078208)); 
}

void FNN_15_2_Layer_1_Dense_tanh(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,float x17,float x18,float x19,
    out float h0,out float h1,out float h2,out float h3,out float h4,out float h5,out float h6,out float h7,out float h8,out float h9)
{
    h0 = tanh(x0*(0.204539)+x1*(-0.0178108)+x2*(0.256238)+x3*(-0.129524)+x4*(0.163211)+x5*(-0.379929)+x6*(-0.18791)+x7*(-0.282919)+x8*(-0.0179307)+x9*(-0.151348)+x10*(0.00252391)+x11*(-0.170182)+x12*(-0.11462)+x13*(-0.368897)+x14*(0.107952)+x15*(-0.361255)+x16*(-0.0750383)+x17*(0.526794)+x18*(0.0991457)+x19*(-0.0775908)+(-0.0365881)); 
    h1 = tanh(x0*(0.295235)+x1*(0.375143)+x2*(-0.366836)+x3*(0.0227205)+x4*(-0.327944)+x5*(-0.284949)+x6*(0.0240938)+x7*(0.280145)+x8*(-0.15581)+x9*(-0.512292)+x10*(0.15597)+x11*(0.214314)+x12*(0.0316763)+x13*(-0.0696979)+x14*(-0.0211807)+x15*(0.252614)+x16*(0.303673)+x17*(-0.395392)+x18*(-0.370011)+x19*(0.211002)+(-0.034407)); 
    h2 = tanh(x0*(0.0895819)+x1*(0.104185)+x2*(0.452183)+x3*(0.228288)+x4*(0.288315)+x5*(-0.251933)+x6*(0.132306)+x7*(0.207553)+x8*(0.0986758)+x9*(-0.296649)+x10*(-0.164587)+x11*(-0.345292)+x12*(0.0506723)+x13*(0.244158)+x14*(0.00861633)+x15*(0.356515)+x16*(-0.363569)+x17*(-0.256014)+x18*(-0.123482)+x19*(-0.0668535)+(0.00607948)); 
    h3 = tanh(x0*(0.285076)+x1*(0.185884)+x2*(-0.427269)+x3*(0.406156)+x4*(-0.333028)+x5*(0.212427)+x6*(0.253366)+x7*(0.298936)+x8*(-0.28798)+x9*(0.50499)+x10*(-0.0271621)+x11*(-0.11975)+x12*(0.146889)+x13*(0.0310458)+x14*(-0.208201)+x15*(-0.273878)+x16*(-0.36979)+x17*(-0.250027)+x18*(-0.373298)+x19*(0.326929)+(-0.0761664)); 
    h4 = tanh(x0*(-0.12436)+x1*(0.342487)+x2*(-0.407672)+x3*(0.397061)+x4*(-0.382542)+x5*(-0.311731)+x6*(-0.194435)+x7*(0.369484)+x8*(0.137088)+x9*(-0.197772)+x10*(-0.221191)+x11*(0.271738)+x12*(0.195438)+x13*(0.142783)+x14*(-0.121305)+x15*(-0.224734)+x16*(-0.0882194)+x17*(0.241982)+x18*(0.274946)+x19*(0.211271)+(-0.0225742)); 
    h5 = tanh(x0*(0.141451)+x1*(0.261771)+x2*(-0.192074)+x3*(-0.41116)+x4*(-0.420406)+x5*(-0.25209)+x6*(0.348029)+x7*(0.188509)+x8*(-0.40866)+x9*(-0.117234)+x10*(0.445981)+x11*(0.064048)+x12*(-0.332507)+x13*(-0.309223)+x14*(-0.254169)+x15*(-0.376318)+x16*(-0.169686)+x17*(-0.130288)+x18*(-0.302419)+x19*(-0.219902)+(0.0332389)); 
    h6 = tanh(x0*(0.231635)+x1*(0.354911)+x2*(-0.307053)+x3*(0.375427)+x4*(-0.344816)+x5*(-0.0336526)+x6*(0.108396)+x7*(-0.354013)+x8*(0.212679)+x9*(-0.437116)+x10*(-0.333597)+x11*(0.318624)+x12*(0.11101)+x13*(0.000776048)+x14*(-0.229609)+x15*(-0.247644)+x16*(-0.272814)+x17*(-0.314335)+x18*(0.374164)+x19*(0.0893684)+(-0.0143295)); 
    h7 = tanh(x0*(0.0425358)+x1*(-0.338522)+x2*(0.286216)+x3*(0.274047)+x4*(0.0760738)+x5*(-0.367723)+x6*(0.295597)+x7*(0.502715)+x8*(0.146279)+x9*(0.435156)+x10*(0.177499)+x11*(-0.274155)+x12*(-0.0773568)+x13*(0.379049)+x14*(-0.231087)+x15*(-0.0277513)+x16*(-0.386413)+x17*(0.0900188)+x18*(-0.0560265)+x19*(0.306872)+(-0.0283628)); 
    h8 = tanh(x0*(0.166614)+x1*(0.116206)+x2*(0.200556)+x3*(0.131288)+x4*(-0.0539128)+x5*(0.389497)+x6*(0.0297987)+x7*(0.0614185)+x8*(-0.436637)+x9*(0.215829)+x10*(-0.0612952)+x11*(0.212253)+x12*(-0.0838735)+x13*(0.157531)+x14*(0.0031906)+x15*(0.435728)+x16*(0.412085)+x17*(0.30345)+x18*(-0.247763)+x19*(0.312283)+(-0.045362)); 
    h9 = tanh(x0*(-0.148988)+x1*(0.178978)+x2*(0.0995551)+x3*(-0.269621)+x4*(-0.314957)+x5*(0.0732658)+x6*(0.290481)+x7*(-0.157072)+x8*(0.233005)+x9*(-0.400229)+x10*(-0.453216)+x11*(-0.401776)+x12*(-0.214349)+x13*(-0.496423)+x14*(0.3349)+x15*(-0.370098)+x16*(0.104664)+x17*(-0.0315531)+x18*(-0.215769)+x19*(0.086343)+(-0.0456665)); 
}

void FNN_15_2_Layer_2_Dense_Identity(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,
    out float h0,out float h1,out float h2)
{
    h0 = x0*(-0.565877)+x1*(-0.431873)+x2*(0.394481)+x3*(-0.463669)+x4*(0.120843)+x5*(0.487333)+x6*(-0.196023)+x7*(-0.486495)+x8*(0.145528)+x9*(-0.0965085)+(0.038463); 
    h1 = x0*(0.0437419)+x1*(-0.228986)+x2*(-0.0955678)+x3*(-0.328304)+x4*(0.0609763)+x5*(0.234672)+x6*(0.0941121)+x7*(0.0310614)+x8*(0.103301)+x9*(-0.189945)+(0.0223106); 
    h2 = x0*(0.145133)+x1*(0.44206)+x2*(-0.53293)+x3*(-0.224629)+x4*(-0.390167)+x5*(0.0114731)+x6*(0.0865357)+x7*(0.525923)+x8*(-0.186607)+x9*(-0.204261)+(0.0447047); 
}

void FNN_15_2(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h_3_0,out float h_3_1,out float h_3_2)
{
    float h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19;
    FNN_15_2_Layer_0_Dense_tanh(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19);

    float h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9;
    FNN_15_2_Layer_1_Dense_tanh(h_1_0,h_1_1,h_1_2,h_1_3,h_1_4,h_1_5,h_1_6,h_1_7,h_1_8,h_1_9,h_1_10,h_1_11,h_1_12,h_1_13,h_1_14,h_1_15,h_1_16,h_1_17,h_1_18,h_1_19,h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9);

    FNN_15_2_Layer_2_Dense_Identity(h_2_0,h_2_1,h_2_2,h_2_3,h_2_4,h_2_5,h_2_6,h_2_7,h_2_8,h_2_9,h_3_0,h_3_1,h_3_2);

}

void ModelKDTree_2(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    FNN_15_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
}

void ModelKDTree_0(float x0,float x1,float x2,float x3,float x4,float x5,float x6,float x7,float x8,float x9,float x10,float x11,float x12,float x13,float x14,float x15,float x16,
    out float h0,out float h1,out float h2)
{
    float interpolateExtent = 1* interpolateRatio;
    float delta = interpolateExtent / 2;
    float lowBound = 0.5 - delta;
    float highBound = 0.5 + delta;
    if ( x8 < lowBound ) {
        ModelKDTree_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);
    }
    else if ( x8 < highBound ) {
        float left_h0;
        float left_h1;
        float left_h2;
        float right_h0;
        float right_h1;
        float right_h2;
        
        ModelKDTree_1(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,left_h0,left_h1,left_h2);
        ModelKDTree_2(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,right_h0,right_h1,right_h2);
        
        float t = 0.5 + ( x8 - 0.5 ) / interpolateExtent;
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
    x0 = (x0 - (0.077087)) / (0.44928);
    x1 = (x1 - (0.025696)) / (0.149719);
    x2 = (x2 - (0.025696)) / (0.149719);
    x3 = (x3 - (-1)) / (0.000488);
    x4 = (x4 - (6.3e-05)) / (1.49896);
    x5 = (x5 - (-0.999512)) / (1.99902);
    x6 = (x6 - (0.090637)) / (0.908875);
    x7 = (x7 - (-0.938477)) / (1.78369);
    x8 = (x8 - (-0.942383)) / (1.90674);
    x9 = (x9 - (1)) / (1);
    x10 = (x10 - (0)) / (1);
    x11 = (x11 - (0)) / (1);
    x12 = (x12 - (0.599609)) / (1);
    x13 = (x13 - (0.199951)) / (1);
    x14 = (x14 - (0.199951)) / (1);
    x15 = (x15 - (20)) / (1);
    x16 = (x16 - (1)) / (1);

    ModelKDTree_0(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,h0,h1,h2);

    // map back
    h0 = h0 * (0.44928) + (0.077087);
    h1 = h1 * (0.149719) + (0.025696);
    h2 = h2 * (0.149719) + (0.025696);
}

