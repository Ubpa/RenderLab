#include <CppUtil/Basic/UGM/Point2.h>
#include <CppUtil/Basic/UGM/Vector1.h>
#include <CppUtil/Basic/UGM/Vector3.h>
#include <CppUtil/Basic/UGM/RGB.h>
#include <CppUtil/Basic/UGM/Normal.h>

#include <CppUtil/Basic/Timer.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include <sstream>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace std;

int main() {
	Point2 p2(-3, 4);

	cout << sizeof(Point2) << endl;
	cout << p2.DistanceWith({ -3,6 }) << endl;
	cout << p2.Abs() << endl;
	
	Vec2 v0(-3, 4);
	Vec2 v1(4, -3);
	Vec2 v2(1, 1);

	cout << sizeof(Vec2) << endl;
	cout << Vec2::Zero_Linearity() << endl;
	constexpr auto identity = Vec2::Identiry_Linearity();
	cout << identity << endl;

	cout << "v0+v1 == v1+v0 : " << ((v0 + v1) == (v1 + v0)) << endl;

	cout << "v0+(v1+v2) = (v0+v1)+v2 : " << ((v0 + (v1 + v2))== (v0 + v1) + v2) << endl;

	cout << "v0 + Vec2::Zero() == v0 : " << (v0 + Vec2::Zero_Linearity()==v0) << endl;

	cout << "v0 + (-v0) == Vec2::Zero() : " << ((v0 + (-v0)) == Vec2::Zero_Linearity()) << endl;

	cout << "Vec2::Identity() * v0 == v0 : " << (Vec2::Identiry_Linearity() * v0 == v0) << endl;

	float k = 0.1f;
	float l = 0.314f;

	cout << "(k*l)*v0==k*(l*v0) : " << ((k*l)*v0 == k * (l*v0)) << endl;

	cout << "(k+l)*v0==k*v0+l*v0 : " << ((k + l)*v0 == k * v0 + l * v0) << endl;

	cout << "k*(v0+v1) == k * v0 + k * v1 : " << (k*(v0 + v1) == k * v0 + k * v1) << endl;
	
	{
		Vec1 v(3.f);
		cout << v.LerpWith(Vec1(4.f), 0.5f) << endl;
	}

	{
		Vec3 v(3, 1, 3);
		v.NormalizeSelf();
		cout << v << endl;
		cout << v.Normalize() << endl;
	}

	{
		RGBf light(0.8, 0.4, 0.4);
		RGBf material(0.4, 0.8, 0.8);
		cout << light * material << endl;
		cout << "(light.Inverse_HadamardProduct() * light == RGBf::Indentity_HadamardProduct()) : " << (light.Inverse_HadamardProduct() * light == RGBf::Indentity_HadamardProduct()) << endl;
		cout << light * 0.5f << endl;
	}

	{
		Normalf n1(1, 1, 0);
		Normalf n2(-1, 1, 0);
		cout << n1.Cross(n2) << endl;
	}

	/*
	{// ÐÔÄÜ²âÊÔ
		{// Cross
			stringstream ss;
			ss << "Cross" << endl;
			{
				Timer timer;
				glm::vec3 v(1, 2, 3);
				glm::vec3 w(4, 5, 6);
				timer.Start();
				for (int i = 0; i < 500000000; i++) {
					v = glm::cross(v, w);
				}
				timer.Stop();
				ss << v.x << v.y << v.z << endl;
				ss << "glm : " << timer.GetWholeTime() << endl;
			}
			{
				Timer timer;
				Vec3 v(7, 8, 9);
				Vec3 w(10, 11, 12);
				timer.Start();
				for (int i = 0; i < 500000000; i++) {
					v = Vec3::Cross(v, w);
				}
				timer.Stop();
				ss << v << endl;
				ss << "my : " << timer.GetWholeTime() << endl;
			}
			cout << ss.str() << endl;
		}
		{// Normalize
			stringstream ss;
			ss << "Normalize" << endl;
			{
				Timer timer;
				glm::vec3 v(1, 2, 3);
				timer.Start();
				for (int i = 0; i < 100000000; i++) {
					v = glm::normalize(v);
				}
				timer.Stop();
				ss << v.x << v.y << v.z << endl;
				ss << "glm : " << timer.GetWholeTime() << endl;
			}
			{
				Timer timer;
				Vec3 v(7, 8, 9);
				timer.Start();
				for (int i = 0; i < 100000000; i++) {
					v = v.Normalize();
				}
				timer.Stop();
				ss << v << endl;
				ss << "my : " << timer.GetWholeTime() << endl;
			}
			cout << ss.str() << endl;
		}
		{// cosTheta
			stringstream ss;
			ss << "CosTheta" << endl;
			{
				Timer timer;
				glm::vec3 v(1, 2, 3);
				glm::vec3 w(4, 5, 6);
				timer.Start();
				for (int i = 0; i < 10000000; i++) {
					v *= glm::dot(v, w) / (glm::length(v)*glm::length(w));
				}
				timer.Stop();
				ss << v.x << v.y << v.z << endl;
				ss << "glm : " << timer.GetWholeTime() << endl;
			}
			{
				Timer timer;
				Vec3 v(7, 8, 9);
				Vec3 w(10, 11, 12);
				timer.Start();
				for (int i = 0; i < 10000000; i++) {
					v *= Vec3::CosTheta(v, w);
				}
				timer.Stop();
				ss << v << endl;
				ss << "my : " << timer.GetWholeTime() << endl;
			}
			cout << ss.str() << endl;
		}
	}
	*/
	return 0;
}
