#include <CppUtil/RTX/OpTexture.h>
#include <CppUtil/Basic/Perlin.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

OpTexture::OpTexture(const function<glm::vec3(float u, float v, const vec3 & p)> & op)
	: op(op) { }

vec3 OpTexture::Value(float u, float v, const vec3 & p) const {
	if (op == NULL)
		return vec3(0);
	
	return op(u, v, p);
}

const Texture::CPtr OpTexture::ConstantTexture(const vec3 & color) {
	return ToPtr(new OpTexture([=](float u, float v, const vec3 & p) -> vec3 {
		return color;
	}));
}

const Texture::CPtr OpTexture::CheckerTexture(const glm::vec3 & color0, const glm::vec3 & color1, float scale) {
	return ToPtr(new OpTexture([=](float u, float v, const vec3 & p) -> vec3 {
		float sines = sinf(scale * p.x) * sinf(scale * p.y) * sinf(scale * p.z);
		return sines < 0 ? color0 : color1;
	}));
}

const Texture::CPtr OpTexture::NoiseTexture(size_t mode, const glm::vec3 & color, float scale) {
	return ToPtr(new OpTexture([=](float u, float v, const vec3 & p) -> vec3 {
		vec3 rst;
		switch (mode)
		{
		default:
		case 0:
			rst = color *0.5f*(1.0f + sin(scale*p.x + 5 * Math::Perlin::Turb(scale*p)));
			break;
		case 1:
			rst = color * 0.5f*(1.0f + sin(5 * Math::Perlin::Turb(scale*p)));
			break;
		case 2:
			rst = color *0.5f*(1.0f + Math::Perlin::Turb(scale * p));
			break;
		case 3:
			rst = color * Math::Perlin::Turb(scale * p);
			break;
		}
		return rst;
	}));
}