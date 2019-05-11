#include <CppUtil/Engine/CmptCamera.h>
#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Basic/UGM/Transform.h>

#include <CppUtil/Basic/Math.h>

#include <iostream>
#include <string>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

int main() {
	auto camera = CmptCamera::New(nullptr);
	camera->InitCoordinate();
	auto ray = camera->GenRay(0.5f, 0.5f);

	const auto tsfm = Transform::Rotate(Vec3(0, 1, 0), 90) * Transform::Translate(Vec3(1,0,0));
	tsfm.ApplyTo(ray);

	cout << "[ correct answer ]" << endl
		<< "origin : [ 0,  0, -1]" << endl
		<< "dir    : [-1,  0,  0]" << endl;

	cout << "[ result ]" << endl
		<< ray << endl;

	return 0;
}
