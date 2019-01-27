#include <CppUtil/RTX/Material.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;

Vertex::Vertex(vec3 pos, vec3 normal, float u, float v) 
	: pos(pos), normal(normal), u(u), v(v){ }

void Vertex::Transform(const mat4 & transform) {
	auto posQ = transform * vec4(pos,1.0);
	pos = vec3(posQ) / posQ.w;
	normal = normalize(transpose(inverse(mat3(transform))) * normal);
}

void Vertex::Transform(const mat4 & transform, const mat3 & normalTransform) {
	auto posQ = transform * vec4(pos, 1.0);
	pos = vec3(posQ) / posQ.w;
	normal = normalize(normalTransform * normal);
}

const Vertex Vertex::Interpolate_Tri(const vec3 & abg, const Vertex & A, const Vertex & B, const Vertex & C) {
	Vertex rst;

	rst.u = dot(abg, vec3(A.u, B.u, C.u));
	rst.v = dot(abg, vec3(A.v, B.v, C.v));
	rst.pos = abg[0] * A.pos + abg[1] * B.pos + abg[2] * C.pos;
	rst.normal = abg[0] * A.normal + abg[1] * B.normal + abg[2] * C.normal;

	return rst;
}

HitRecord::HitRecord(Ptr<Ray> ray, const vec3 & pos, const vec3 & normal, float u, float v)
	:ray(ray), vertex(pos,normal,u,v) { }