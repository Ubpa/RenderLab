#include <CppUtil/RTX/Metal.h>

#include <CppUtil/RTX/ConstTexture.h>
#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;


Metal::Metal(Texture::CPtr specular, float fuzz)
	: specular(specular), fuzz(fuzz) { }

Metal::Metal(float r, float g, float b, float fuzz)
	: Metal(rgb(r,g,b), fuzz) { }

Metal::Metal(const glm::rgb & specular, float fuzz)
	: Metal(ToPtr(new ConstTexture(specular)), fuzz) { }

bool Metal::Scatter(const HitRecord & rec) const {
	vec3 dir = reflect(rec.ray->GetDir(), rec.vertex.normal);
	vec3 dirFuzz = dir + fuzz * Math::RandInSphere();

	// 反射光线在表面之下
	if (dot(dirFuzz, rec.vertex.normal) < 0) {
		rec.ray->SetLightColor(vec3(0));
		return false;
	}

	auto attenuation = specular->Value(rec.vertex.u, rec.vertex.v, rec.vertex.pos);
	rec.ray->Update(rec.vertex.pos, dirFuzz, attenuation);
	return true;
}