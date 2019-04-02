#include <CppUtil/Engine/BSDF_Diffuse.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Image.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const RGBf BSDF_Diffuse::F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	if (wo.z <= 0 || wi.z <= 0)
		return RGBf(0.f);
	
	return GetAlbedo(texcoord) / Math::PI;
}

const RGBf BSDF_Diffuse::Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) {
	if (wo.z <= 0) {
		PD = 0;
		wi = Normalf(0.f);
		return RGBf(0.f);
	}

	wi = sampler.GetSample(PD);

	return GetAlbedo(texcoord) / Math::PI;
}

float BSDF_Diffuse::PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	return wi.z > 0 && wo.z > 0 ? wi.z / Math::PI : 0;
}

const RGBf BSDF_Diffuse::GetAlbedo(const Point2 & texcoord) const {
	if (!albedoTexture || !albedoTexture->IsValid())
		return colorFactor;

	return colorFactor * albedoTexture->Sample(texcoord, Image::Mode::BILINEAR).ToRGB();
}
