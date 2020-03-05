#include <Engine/BSDF_Diffuse.h>

#include <Basic/Math.h>
#include <Basic/Image.h>

using namespace Ubpa;

const rgbf BSDF_Diffuse::F(const normalf & wo, const normalf & wi, const pointf2 & texcoord) {
	if (wo[2] <= 0 || wi[2] <= 0)
		return rgbf(0.f);
	
	return GetAlbedo(texcoord) / PI<float>;
}

const rgbf BSDF_Diffuse::Sample_f(const normalf & wo, const pointf2 & texcoord, normalf & wi, float & PD) {
	if (wo[2] <= 0) {
		PD = 0;
		wi = normalf(0.f);
		return rgbf(0.f);
	}

	wi = sampler.GetSample(PD).cast_to<normalf>();

	return GetAlbedo(texcoord) / PI<float>;
}

float BSDF_Diffuse::PDF(const normalf & wo, const normalf & wi, const pointf2 & texcoord) {
	return wi[2] > 0 && wo[2] > 0 ? wi[2] / PI<float> : 0;
}

const rgbf BSDF_Diffuse::GetAlbedo(const pointf2 & texcoord) const {
	if (!albedoTexture || !albedoTexture->IsValid())
		return colorFactor;

	return colorFactor * albedoTexture->Sample(texcoord, Image::Mode::BILINEAR).to_rgb();
}
