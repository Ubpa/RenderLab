#include <CppUtil/Basic/Vector.h>
#include <CppUtil/Basic/Point.h>
#include <CppUtil/Basic/Val2.h>
#include <CppUtil/Basic/Normal.h>
#include <CppUtil/Basic/Transform.h>
#include <CppUtil/Basic/Mat3x3.h>
#include <CppUtil/Basic/EulerYXZ.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace std;

int main() {
	EulerYXZf euler(45, 0, 0);
	Point3 p(0, 0, 1);
	
	auto q = euler.ToQuat();
	cout << q * p << endl;
	cout << Transform::Rotate(q)(p) << endl;
	cout << Transform::Rotate(q).RotationQuat() * p << endl;
	cout << Transform::Rotate(Transform::Rotate(q).RotationEulerYXZ())(p) << endl;

	return 0;
}
