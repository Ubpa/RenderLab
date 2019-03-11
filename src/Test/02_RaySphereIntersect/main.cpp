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
	auto intersector = ToPtr(new RayIntersector(ray));

	auto sphere = ToPtr(new Sphere(vec3(0), 1));
	sphere->Accept(intersector);

	if (intersector->GetRst().IsIntersect()) {
		std::cout
			<< "intersect" << std::endl
			// (0, 0, 1)
			<< "Hit Point is : " << to_string(ray->EndPos()) << std::endl;
	}
	else
		std::cout << "not intersect" << std::endl;

	return 0;
}