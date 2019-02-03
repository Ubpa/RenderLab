#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Basic/Math.h>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

int main() {
	Camera::Ptr camera = ToPtr(new Camera(nullptr));
	auto ray = camera->GenRay(0.5f, 0.5f);
	mat4 mat(1.0f);
	mat = rotate(mat, 0.5f * Math::PI, vec3(0, 1, 0));
	mat = translate(mat, vec3(1, 0, 0));
	ray->Transform(mat);

	cout << "[ correct answer ]" << endl
		<< "origin : ( 0,  0, -1)" << endl
		<< "dir    : (-1,  0,  0)" << endl;

	cout << "[ result ]" << endl
		<< "origin : " << to_string(ray->GetOrigin()) << endl
		<< "dir    : " << to_string(ray->GetDir()) << endl;

	return 0;
}
