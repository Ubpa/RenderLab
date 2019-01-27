#include <CppUtil/RTX/RayTracer.h>

#include <CppUtil/RTX/Ray.h>

using namespace RTX;
using namespace glm;

RayTracer::RayTracer(size_t depth) 
	: depth(depth), wholeDepth(0) { }

rgb RayTracer::TraceX(Hitable::CPtr scene, CppUtil::Basic::Ptr<Ray> & ray) {
	return TraceX(scene, ray, depth);
}

rgb RayTracer::TraceX(Hitable::CPtr scene, CppUtil::Basic::Ptr<Ray> & ray, size_t depth) {
	if (depth == 0) {
		wholeDepth += this->depth + 1;
		return rgb(1.0001f / 255.0f);
	}

	auto hitRst = scene->RayIn(ray);
	if (hitRst.hit) {
		if (hitRst.material == NULL) {
			wholeDepth += this->depth - depth + 1;
			return rgb(1, 0, 1);
		}

		if (hitRst.material->Scatter(hitRst.record))
			return RayTracer::TraceX(scene, ray, depth - 1);
		else {
			wholeDepth += this->depth - depth + 1;
			return ray->GetColor();
		}
	}
	else {
		wholeDepth += this->depth - depth + 1;
		return rgb(1.0001f / 255.0f);
	}
}

rgb RayTracer::Trace(Hitable::CPtr scene, CppUtil::Basic::Ptr<Ray> & ray, size_t depth) {
	if (depth == 0)
		return rgb(1.0001f / 255.0f);

	auto hitRst = scene->RayIn(ray);
	if (hitRst.hit) {
		if (hitRst.material == NULL)
			return rgb(1, 0, 1);

		if (hitRst.material->Scatter(hitRst.record))
			return RayTracer::Trace(scene, ray, depth - 1);
		else 
			return ray->GetColor();
	}
	else
		return rgb(1.0001f / 255.0f);
}