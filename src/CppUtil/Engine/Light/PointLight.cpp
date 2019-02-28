#include <CppUtil/Engine/PointLight.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

vec3 PointLight::Sample_L(const vec3& p, vec3 & wi, float & distToLight, float & PD) const {
	float dist2 = dot(p, p);
	distToLight = sqrt(dist2);
	wi = - p / distToLight;
	PD = 1.0f;
	return intensity * color / (1.0f + linear * distToLight + quadratic * dist2);
}
