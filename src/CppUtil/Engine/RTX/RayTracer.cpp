#include <CppUtil/Engine/RayTracer.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/RayIntersector.h>
#include <CppUtil/Engine/Transform.h>
#include <CppUtil/Engine/Geometry.h>

using namespace CppUtil::Engine;
using namespace glm;

RayTracer::RayTracer(Scene::Ptr scene)
	: scene(scene) { }

// ray 所处的坐标系应该是 sobj 所处的坐标系
RayTracer::Rst RayTracer::FindClosetSObj(SObj::Ptr sobj, Ray::Ptr ray) {
	auto rayIntersector = ToPtr(new RayIntersector);
	rayIntersector->SetRay(ray);
	Rst rst;
	FindClosetSObj(sobj, ray, rayIntersector, rst);
	return rst;
}

void RayTracer::FindClosetSObj(SObj::Ptr sobj, Ray::Ptr ray, RayIntersector::Ptr rayIntersector, Rst & closestRst) {
	auto transform = sobj->GetComponent<Transform>();
	if (transform)
		ray->Transform(transform->GetInv());

	auto geometry = sobj->GetComponent<Geometry>();
	if (geometry) {
		geometry->GetPrimitive()->Accept(rayIntersector);
		auto rst = rayIntersector->GetRst();

		if (rst.isIntersect) {
			ray->SetTMax(rst.t);
			closestRst.closestSObj = sobj;
			closestRst.n = rst.n;
		}
	}

	for (auto child : sobj->GetChildren())
		FindClosetSObj(child, ray, rayIntersector, closestRst);

	if (transform) {
		ray->Transform(transform->GetMat());
		if (closestRst.closestSObj == sobj)
			closestRst.n = normalize(transform->GetNormMat()*closestRst.n);
	}
}
