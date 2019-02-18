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

vec3 RayTracer::Trace(Ray::Ptr & ray, int depth) {
	const float p = depth < 8 ? 1.0f : 0.98f;
	if (Math::Rand_F() > p)
		return vec3(0);

	auto hitRst = scene->RayIn(ray);
	if (hitRst.hit) {
		if (hitRst.material == nullptr)
			return vec3(1, 0, 1);

		if (hitRst.material->Scatter(hitRst.record))
			return Trace(ray, depth + 1) / p;
		else
			return ray->GetColor() / p;
	}
	else
		return vec3(0);
}