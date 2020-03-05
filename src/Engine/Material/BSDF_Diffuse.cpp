#include <Engine/BSDF_Diffuse.h>

#include <Basic/Math.h>
#include <Basic/Image.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const Ubpa::rgbf BSDF_Diffuse::F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) {
	if (wo[2] <= 0 || wi[2] <= 0)
		return Ubpa::rgbf(0.f);
	
	return GetAlbedo(texcoord) / Ubpa::PI<float>;
}

const Ubpa::rgbf BSDF_Diffuse::Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) {
	if (wo[2] <= 0) {
		PD = 0;
		wi = Ubpa::normalf(0.f);
		return Ubpa::rgbf(0.f);
	}

	wi = sampler.GetSample(PD).cast_to<Ubpa::normalf>();

	return GetAlbedo(texcoord) / Ubpa::PI<float>;
}

float BSDF_Diffuse::PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) {
	return wi[2] > 0 && wo[2] > 0 ? wi[2] / Ubpa::PI<float> : 0;
}

const Ubpa::rgbf BSDF_Diffuse::GetAlbedo(const Ubpa::pointf2 & texcoord) const {
	if (!albedoTexture || !albedoTexture->IsValid())
		return colorFactor;

	return colorFactor * albedoTexture->Sample(texcoord, Image::Mode::BILINEAR).to_rgb();
}
