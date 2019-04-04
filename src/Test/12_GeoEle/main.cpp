#include <CppUtil/Basic/UGM/UGM.h>

#include <CppUtil/Basic/Timer.h>

#include <sstream>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace std;

int main() {
	cout << sizeof(Vec2) << endl;
	cout << sizeof(Vec3) << endl;
	cout << sizeof(Vec4) << endl;

	EulerYXZf euler(45, 0, 0);
	Point3 p(0, 0, 1);
	
	auto q = euler.ToQuat();
	cout << q * p << endl;
	cout << Transform::Rotate(q)(p) << endl;
	cout << Transform::Rotate(q).RotationQuat() * p << endl;
	cout << Transform::Rotate(Transform::Rotate(q).RotationEulerYXZ())(p) << endl;

	cout << euler.MaxComponent() << endl;

	Vec2 c(-1, 2);

	cout << c.Abs() << endl;
	cout << c.LerpWith({ 1,2 }, 0.5f) << endl;

	cout << Vec3(3, 4, 5).MidWith(Vec3(4, 5, 6)) << endl;

	RGBf color(0.5, 0.3, 0.3);
	cout << color.MidWith({ 0.6f }) << endl;

	return 0;
}
