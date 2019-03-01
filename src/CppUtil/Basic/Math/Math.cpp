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

vec2 Math::SphereNormal2Texcoord(const vec3 & normal) {
	vec2 uv;
	float phi = atan2(-normal.x, -normal.z) + Math::PI;
	float theta = acos(normal.y);

	uv[0] = phi / (2 * PI);
	uv[1] = theta / PI;
	return uv;
}

vec3 Math::SphereNormal2Tangent(const vec3 & normal) {
	float phi = atan2(-normal.x, -normal.z) + Math::PI;

	return vec3(cos(phi), 0, - sin(phi));
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

bool Math::IsBase2(int n) {
	while (n > 1) {
		if (n & 0x1)
			return false;

		n >>= 1;
	}
	return true;
}

mat3 Math::GenCoordSpace(const vec3 & n) {
	auto z = normalize(n);
	auto h = z;
	if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z))
		h.x = 1.0;
	else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z))
		h.y = 1.0;
	else
		h.z = 1.0;

	auto y = normalize(cross(h, z));
	auto x = normalize(cross(z, y));

	mat3 o2w;
	o2w[0] = x;
	o2w[1] = y;
	o2w[2] = z;

	return o2w;
}

float Math::Illum(const vec3 & color) {
	return 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
}