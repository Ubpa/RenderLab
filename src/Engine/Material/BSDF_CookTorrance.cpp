#include <Engine/Material/BSDF_CookTorrance.h>

#include <Basic/Math.h>

using namespace Ubpa;

using namespace std;

float BSDF_CookTorrance::NDF(const normalf & h) {
	// backmann

	//float NoH = h[2];
	float NoH2 = h[2] * h[2];
	float NoH4 = NoH2 * NoH2;
	float m2 = m * m;
	return exp((NoH2 - 1) / (m2 * NoH2)) / (m2 * NoH4);
}

float BSDF_CookTorrance::Fr(const normalf & wi, const normalf & h) {
	// schlick

	float R0 = (ior - 1) / (ior + 1);
	R0 *= R0;
	return R0 + (1 - R0)*pow(1 - wi.dot(h), 5);
}

float BSDF_CookTorrance::G(const normalf & wo, const normalf & wi, const normalf & h){
	// Cook-Torrance

	//float NoH = h[2];
	float HoWo = h.dot(wo);
	//float NoWo = wo[2];
	//float NoWi = wi[2];
	float commomPart = 2 * h[2] / HoWo;
	float item1 = commomPart * wo[2];
	float item2 = commomPart * wi[2];
	return min(min(1.0f, item1), item2);
}

const rgbf BSDF_CookTorrance::F(const normalf & wo, const normalf & wi, const pointf2 & texcoord) {
	const normalf h = (wo + wi).normalize();
	float fr = Fr(wi, h);
	float kd = 1 - fr;
	return kd * albedo / PI<float> + NDF(h) * fr * G(wo, wi, h) / (4 * wo[2] *wi[2]) * refletance;
}

float BSDF_CookTorrance::PDF(const normalf & wo, const normalf & wi, const pointf2 & texcoord) {
	//vec3 h = normalize(wo + wi);
	//return NDF(h) / 4.0f;

	return 1.0f / (2.0f * PI<float>);
}

const rgbf BSDF_CookTorrance::Sample_f(const normalf & wo, const pointf2 & texcoord, normalf & wi, float & pd) {
	/*
	// 根据 NDF 来进行重要性采样
	// 有异常，暂时未解决
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();

	float cosTheta = sqrt(1.0f / (1.0f - m * m * log(1 - Xi1)));
	float phi = 2 * PI<float> * Xi2;
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	vec3 h(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
	wi = reflect(-wo, h);

	float ndf = NDF(h);
	pd = ndf / 4.0f;
	*/
	
	// 均匀采样
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();
	float cosTheta = Xi1;
	float sinTheta = sqrt(1 - cosTheta * cosTheta);
	float phi = 2 * PI<float> * Xi2;
	wi = normalf(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
	normalf h = (wo + wi).normalize();
	pd = 1.0f / (2.0f * PI<float>);

	float fr = Fr(wi, h);
	float kd = 1 - fr;
	return kd*albedo/PI<float> + NDF(h) * fr * G(wo, wi, h) / (4 * wo[2] *wi[2]) * refletance;
}
