#include <CppUtil/Engine/BSDF_Diffuse.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Image.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

vec3 BSDF_Diffuse::F(const vec3 & wo, const vec3 & wi, const vec2 & texcoord) {
	if (wo.z <= 0 || wi.z <= 0)
		return vec3(0);
	
	return GetAlbedo(texcoord) / Math::PI;
}

vec3 BSDF_Diffuse::Sample_f(const vec3 & wo, const vec2 & texcoord, vec3 & wi, float & PD) {
	if (wo.z <= 0) {
		PD = 0;
		wi = vec3(0);
		return vec3(0);
	}

	wi = sampler.GetSample(PD);

	return GetAlbedo(texcoord) / Math::PI;
}

float BSDF_Diffuse::PDF(const vec3 & wo, const vec3 & wi, const vec2 & texcoord) {
	return wi.z > 0 && wo.z > 0 ? wi.z / Math::PI : 0;
}

const vec3 BSDF_Diffuse::GetAlbedo(const vec2 & texcoord) const {
	if (!albedoTexture || !albedoTexture->IsValid())
		return albedoColor;

	bool blend = albedoTexture->GetChannel() == 4;
	return vec3(albedoTexture->Sample(texcoord, blend))*albedoColor;
}
