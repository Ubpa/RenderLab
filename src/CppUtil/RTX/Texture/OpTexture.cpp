#include <CppUtil/RTX/OpTexture.h>
#include <CppUtil/Basic/Perlin.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

OpTexture::OpTexture(const function<glm::rgb(float u, float v, const vec3 & p)> & op)
	: op(op) { }

rgb OpTexture::Value(float u, float v, const vec3 & p) const {
	if (op == NULL)
		return rgb(0);
	
	return op(u, v, p);
}

const Texture::CPtr OpTexture::ConstantTexture(const rgb & color) {
	return ToPtr(new OpTexture([=](float u, float v, const vec3 & p) -> rgb {
		return color;
	}));
}

const Texture::CPtr OpTexture::CheckerTexture(const glm::rgb & color0, const glm::rgb & color1, float scale) {
	return ToPtr(new OpTexture([=](float u, float v, const vec3 & p) -> rgb {
		float sines = sinf(scale * p.x) * sinf(scale * p.y) * sinf(scale * p.z);
		return sines < 0 ? color0 : color1;
	}));
}

const Texture::CPtr OpTexture::NoiseTexture(size_t mode, const glm::rgb & color, float scale) {
	return ToPtr(new OpTexture([=](float u, float v, const vec3 & p) -> rgb {
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