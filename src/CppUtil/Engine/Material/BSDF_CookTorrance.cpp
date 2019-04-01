#include <CppUtil/Engine/BSDF_CookTorrance.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

float BSDF_CookTorrance::NDF(const Normalf & h) {
	// backmann

	//float NoH = h.z;
	float NoH2 = h.z * h.z;
	float NoH4 = NoH2 * NoH2;
	float m2 = m * m;
	return exp((NoH2 - 1) / (m2 * NoH2)) / (m2 * NoH4);
}

float BSDF_CookTorrance::Fr(const Normalf & wi, const Normalf & h) {
	// schlick

	float R0 = (ior - 1) / (ior + 1);
	R0 *= R0;
	return R0 + (1 - R0)*pow(1 - wi.Dot(h), 5);
}

float BSDF_CookTorrance::G(const Normalf & wo, const Normalf & wi, const Normalf & h){
	// Cook-Torrance

	//float NoH = h.z;
	float HoWo = h.Dot(wo);
	//float NoWo = wo.z;
	//float NoWi = wi.z;
	float commomPart = 2 * h.z / HoWo;
	float item1 = commomPart * wo.z;
	float item2 = commomPart * wi.z;
	return min(min(1.0f, item1), item2);
}

const RGBf BSDF_CookTorrance::F(const Normalf & wo, const Normalf & wi, const Point2f & texcoord) {
	const Normalf h = (wo + wi).Norm();
	float fr = Fr(wi, h);
	float kd = 1 - fr;
	return kd * albedo / Math::PI + NDF(h) * fr * G(wo, wi, h) / (4 * wo.z *wi.z) * refletance;
}

float BSDF_CookTorrance::PDF(const Normalf & wo, const Normalf & wi, const Point2f & texcoord) {
	//vec3 h = normalize(wo + wi);
	//return NDF(h) / 4.0f;

	return 1.0f / (2.0f * Math::PI);
}

const RGBf BSDF_CookTorrance::Sample_f(const Normalf & wo, const Point2f & texcoord, Normalf & wi, float & pd) {
	/*
	// 根据 NDF 来进行重要性采样
	// 有异常，暂时未解决
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();

	float cosTheta = sqrt(1.0f / (1.0f - m * m * log(1 - Xi1)));
	float phi = 2 * Math::PI * Xi2;
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
	float phi = 2 * Math::PI * Xi2;
	wi = Normalf(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
	Normalf h = (wo + wi).Norm();
	pd = 1.0f / (2.0f * Math::PI);

	float fr = Fr(wi, h);
	float kd = 1 - fr;
	return kd*albedo/Math::PI + NDF(h) * fr * G(wo, wi, h) / (4 * wo.z *wi.z) * refletance;
}
