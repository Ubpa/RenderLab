#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/RayIntersector.h>
#include <CppUtil/Engine/Ray.h>

#include <iostream>
#include <string>

using namespace CppUtil;
using namespace CppUtil::Engine;

int main() {
	ERay ray(Point3(0, 0, 2), Vec3(0, 0, -1));
	auto intersector = ToPtr(new RayIntersector);
	intersector->Init(&ray);

	auto sphere = ToPtr(new Sphere);
	sphere->Accept(intersector);

	if (intersector->GetRst().IsIntersect()) {
		std::cout
			<< "intersect" << std::endl
			// (0, 0, 1)
			<< "Hit Point is : " << ray.EndPos() << std::endl;
	}
	else
		std::cout << "not intersect" << std::endl;

	return 0;
}
