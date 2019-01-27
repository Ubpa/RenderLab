#include <CppUtil/RTX/Lambertian.h>

#include <CppUtil/RTX/ConstTexture.h>
#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;

Lambertian::Lambertian(Texture::CPtr albedo)
	: albedo(albedo) { }

Lambertian::Lambertian(float r, float g, float b)
	: Lambertian(rgb(r, g, b)) { }

Lambertian::Lambertian(const rgb & albedo) {
	this->albedo = ToPtr(new ConstTexture(albedo));
}

bool Lambertian::Scatter(const HitRecord & rec) const {
	vec3 dir = rec.vertex.normal + Math::RandInSphere();
	rec.ray->Update(rec.vertex.pos, dir, albedo->Value(rec.vertex.u,rec.vertex.v,rec.vertex.pos));
	return true;
}