#include <CppUtil/Basic/Vector.h>
#include <CppUtil/Basic/Point.h>
#include <CppUtil/Basic/Normal.h>
#include <CppUtil/Basic/Transform.h>

using namespace CppUtil::Basic;
using namespace std;

int main() {
	Vectorf v(1);
	Pointf p0(1, 1, 1), p1(1, 2, 3);
	Normalf n(1, 2, 3);
	Mat4f mat(2);
	Transform tsf(1.f);

	auto view = Transform::LookAt(p0, p1);
	auto perspective = Transform::Perspcetive(60, 16.f / 9.f, 0.01f, 100.f);
	auto w2n = perspective * view;
	auto orthographic = Transform::Orthographic(1, 1, 0.01f, 100.f);


	cout << 3 * v << endl;
	cout << p0 << endl
		<< p1 << endl
		<< typeid(p1 - p0).name() << endl;
	cout << n << endl;
	cout << Pointf::Lerp(p0, p1, 0.5) << endl;
	cout << p0.Distance(p1) << endl;
	cout << v.Cross(p1 - p0) << endl;
	cout << mat << endl;
	cout << mat.Inverse() << endl;
	mat(1,2) = 3;
	cout << mat.Transpose() << endl;
	cout << view.GetMatrix() * view.GetMatrix().Inverse() << endl;
	cout << view.GetInverseMatrix() * view.GetInverseMatrix().Inverse() << endl;
	cout << tsf(p0) << endl;
	cout << view(p0) << endl;

	float dis = 100.f;
	auto p = p0 + (p1 - p0).Norm()*dis;
	cout << p << endl;
	cout << view(p) << endl;
	cout << perspective(view(p)) << endl;
	cout << w2n(p) << endl;
	cout << perspective(Pointf(0, 0, -0.01f)) << endl;
	cout << perspective(Pointf(0, 0, -100.f)) << endl;
	cout << orthographic(Pointf(0, 0, -0.01f)) << endl;
	cout << orthographic(Pointf(0, 0, -100.f)) << endl;

	return 0;
}
