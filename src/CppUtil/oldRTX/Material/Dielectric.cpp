#include <CppUtil/RTX/Dielectric.h>

#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;


Dielectric::Dielectric(float refractIndex, vec3 attenuationConst)
	: refractIndex(refractIndex), attenuationConst(attenuationConst) { }

// 返回值为 true 说明光线继续传播
// 返回值为 false 说明光线不再传播
bool Dielectric::Scatter(const HitRecord & rec) const {
	vec3 refractDir;
	const vec3 reflectDir = reflect(rec.ray->GetDir(), rec.vertex.normal);

	const vec3 & d = rec.ray->GetDir();
	const vec3 & n = rec.vertex.normal;
	vec3 attenuation;
	vec3 airViewDir;
	if (dot(d,n) < 0) {
		Math::Refract(d, n, 1.0f / refractIndex, refractDir);
		attenuation = vec3(1.0f);
		airViewDir = -d;
	}
	else {
		attenuation = exp(-attenuationConst * rec.ray->GetTMax() * length(rec.ray->GetDir()));

		if (!Math::Refract(d, -n, refractIndex, refractDir)) {
			rec.ray->Update(rec.vertex.pos, reflectDir, attenuation);
			return true;
		}
		
		airViewDir = refractDir;
	}

	float fresnelFactor = Math::FresnelSchlick(airViewDir, rec.vertex.normal, refractIndex);
	const vec3 & dir = Math::Rand_F() > fresnelFactor ? refractDir : reflectDir;
	rec.ray->Update(rec.vertex.pos, dir, attenuation);

	return true;
}