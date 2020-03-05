#include <Engine/BSDF_CookTorrance.h>

#include <Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

float BSDF_CookTorrance::NDF(const Ubpa::normalf & h) {
	// backmann

	//float NoH = h[2];
	float NoH2 = h[2] * h[2];
	float NoH4 = NoH2 * NoH2;
	float m2 = m * m;
	return exp((NoH2 - 1) / (m2 * NoH2)) / (m2 * NoH4);
}

float BSDF_CookTorrance::Fr(const Ubpa::normalf & wi, const Ubpa::normalf & h) {
	// schlick

	float R0 = (ior - 1) / (ior + 1);
	R0 *= R0;
	return R0 + (1 - R0)*pow(1 - wi.dot(h), 5);
}

float BSDF_CookTorrance::G(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::normalf & h){
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

const Ubpa::rgbf BSDF_CookTorrance::F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) {
	const Ubpa::normalf h = (wo + wi).normalize();
	float fr = Fr(wi, h);
	float kd = 1 - fr;
	return kd * albedo / Ubpa::PI<float> + NDF(h) * fr * G(wo, wi, h) / (4 * wo[2] *wi[2]) * refletance;
}

float BSDF_CookTorrance::PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) {
	//vec3 h = normalize(wo + wi);
	//return NDF(h) / 4.0f;

	return 1.0f / (2.0f * Ubpa::PI<float>);
}

const Ubpa::rgbf BSDF_CookTorrance::Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & pd) {
	/*
	// 根据 NDF 来进行重要性采样
	// 有异常，暂时未解决
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();

	float cosTheta = sqrt(1.0f / (1.0f - m * m * log(1 - Xi1)));
	float phi = 2 * Ubpa::PI<float> * Xi2;
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
	float phi = 2 * Ubpa::PI<float> * Xi2;
	wi = Ubpa::normalf(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
	Ubpa::normalf h = (wo + wi).normalize();
	pd = 1.0f / (2.0f * Ubpa::PI<float>);

	float fr = Fr(wi, h);
	float kd = 1 - fr;
	return kd*albedo/Ubpa::PI<float> + NDF(h) * fr * G(wo, wi, h) / (4 * wo[2] *wi[2]) * refletance;
}
