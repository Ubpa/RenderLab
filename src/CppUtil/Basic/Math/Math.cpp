#include <CppUtil/Basic/Math.h>
#include <random>

using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

static uniform_int_distribution<unsigned> uiMap;
static uniform_int_distribution<signed> iMap;
static uniform_real_distribution<float> fMap(0.0f,1.0f);
static uniform_real_distribution<double> dMap(0.0,1.0);
static default_random_engine engine;

int Math::Rand_I() {
	return iMap(engine);
}

unsigned int Math::Rand_UI() {
	return uiMap(engine);
}

float Math::Rand_F() {
	return fMap(engine);
}

double Math::Rand_D() {
	return dMap(engine);
}

vec2 Math::RandInCircle() {
	vec2 rst;
	do {
		rst = vec2(fMap(engine), fMap(engine))*2.0f - 1.0f;
	} while (dot(rst, rst) >= 1.0);
	return rst;
}

vec3 Math::RandInSphere() {
	vec3 rst;
	do {
		rst = vec3(fMap(engine), fMap(engine), fMap(engine))*2.0f - 1.0f;
	} while (dot(rst, rst) >= 1.0);
	return rst;
}

float Math::Gray(const vec3 & color) {
	return color.r * 0.299f + color.g * 0.587f + color.b * 0.114f;
}

bool Math::Refract(const vec3 & viewDir, const vec3 & normal, float ratioNiNt, vec3 & refractDir) {
	const vec3 ud = normalize(viewDir);
	float cosTheta = dot(ud, normal);
	float discriminent = 1 - pow(ratioNiNt, 2) * (1 - pow(cosTheta, 2));

	if (discriminent < 0)
		return false;

	refractDir = ratioNiNt * (ud - normal * cosTheta) - normal * sqrt(discriminent);
	return true;
}

float Math::FresnelSchlick(const vec3 & viewDir, const vec3 & halfway, float ratioNtNi) {
	float cosTheta = dot(normalize(viewDir), normalize(halfway));
	float R0 = pow((ratioNtNi - 1) / (ratioNtNi + 1), 2);
	float R = R0 + (1 - R0)*pow(1 - cosTheta, 5);
	return R;
}

vec2 Math::Sphere2UV(const vec3 & normal) {
	vec2 uv;
	float phi = atan2(normal.z, normal.x);
	float theta = asin(normal.y);
	uv[0] = 1 - (phi + PI) / (2 * PI);
	uv[1] = (theta + PI/2) / PI;
	return uv;
}

vec4 Math::Intersect_RayTri(const vec3 & e, const vec3 & d, const vec3 & a, const vec3 & b, const vec3 & c) {
	mat3 equation_A(vec3(a-b), vec3(a-c), d);

	//Æ½ÐÐ
	if (abs(determinant(equation_A)) < EPSILON)
		return vec4(0, 0, 0, 0);

	vec3 equation_b = a - e;
	vec3 equation_X = inverse(equation_A) * equation_b;
	float alpha = 1 - equation_X[0] - equation_X[1];
	return vec4(alpha, equation_X);
}