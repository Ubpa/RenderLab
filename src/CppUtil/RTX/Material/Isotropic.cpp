#include <CppUtil/RTX/Isotropic.h>

#include <CppUtil/RTX/ConstTexture.h>
#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;


Isotropic::Isotropic(const rgb & color)
	: tex(ToPtr(new ConstTexture(color))) { }

bool Isotropic::Scatter(const HitRecord & rec) const {
	auto attenuation = tex->Value(rec.vertex.u, rec.vertex.v, rec.vertex.pos);
	rec.ray->Update(rec.vertex.pos, Math::RandInSphere(), attenuation);
	return true;
}