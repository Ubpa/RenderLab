#include <Utility/RTX/Isotropic.h>

#include <Utility/RTX/ConstTexture.h>
#include <Utility/RTX/Ray.h>

#include <Utility/Basic/Math.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;


Isotropic::Isotropic(const rgb & color)
	: tex(ToPtr(new ConstTexture(color))) { }

bool Isotropic::Scatter(const HitRecord & rec) const {
	auto attenuation = tex->Value(rec.vertex.u, rec.vertex.v, rec.vertex.pos);
	rec.ray->Update(rec.vertex.pos, Math::RandInSphere(), attenuation);
	return true;
}