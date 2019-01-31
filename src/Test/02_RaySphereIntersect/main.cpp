#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/RayIntersector.h>
#include <CppUtil/Engine/Ray.h>

#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>

using namespace CppUtil::Engine;
using namespace glm;

int main() {
	Ray::Ptr ray = ToPtr(new Ray(vec3(0, 0, 2), vec3(0, 0, -1)));
	RayIntersector::Ptr intersector = ToPtr(new RayIntersector(ray));

	Primitive::Ptr sphere = ToPtr(new Sphere(vec3(0), 1));
	sphere->Accept(intersector);

	if (intersector->GetRst()->isIntersect) {
		std::cout
			<< "intersect" << std::endl
			<< "Hit Point is : " << to_string(ray->At(intersector->GetRst()->t)) << std::endl;
	}
	else
		std::cout << "not intersect" << std::endl;

	return 0;
}