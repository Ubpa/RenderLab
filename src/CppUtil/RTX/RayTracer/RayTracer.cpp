#include <CppUtil/RTX/RayTracer.h>

#include <CppUtil/RTX/Hitable.h>
#include <CppUtil/RTX/Material.h>
#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;

RayTracer::RayTracer(Hitable::CPtr scene)
	: scene(scene) { }

vec3 RayTracer::Trace(Ray::Ptr & ray) {
	if (Math::Rand_F() > 0.98)
		return vec3(0);

	auto hitRst = scene->RayIn(ray);
	if (hitRst.hit) {
		if (hitRst.material == NULL)
			return vec3(1, 0, 1);

		if (hitRst.material->Scatter(hitRst.record))
			return Trace(ray);
		else
			return ray->GetColor();
	}
	else
		return vec3(0);
}