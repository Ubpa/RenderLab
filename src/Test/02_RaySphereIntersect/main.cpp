#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/RayIntersector.h>
#include <CppUtil/Engine/Ray.h>

// string_cast
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>

using namespace CppUtil::Engine;
using namespace glm;

int main() {
	auto ray = ToPtr(new Ray(vec3(0, 0, 2), vec3(0, 0, -1)));
	auto intersector = ToPtr(new RayIntersector);
	intersector->SetRay(ray);

	auto sphere = ToPtr(new Sphere(vec3(0), 1));
	sphere->Accept(intersector);

	if (intersector->GetRst().isIntersect) {
		std::cout
			<< "intersect" << std::endl
			<< "Hit Point is : " << to_string(ray->At(intersector->GetRst().t)) << std::endl;
	}
	else
		std::cout << "not intersect" << std::endl;

	return 0;
}