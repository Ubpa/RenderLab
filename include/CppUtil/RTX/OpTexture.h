#ifndef _OP_TEXTURE_H_
#define _OP_TEXTURE_H_

#include <CppUtil/RTX/Texture.h>

#include <functional>

namespace RTX {
	class OpTexture : public Texture {
		TEXTURE_SETUP(OpTexture)
	public:
		OpTexture(const std::function<glm::vec3(float u, float v, const glm::vec3 & p)> & op);

		virtual glm::vec3 Value(float u = 0, float v = 0, const glm::vec3 & p = glm::vec3(0)) const;

		static const Texture::CPtr ConstantTexture(const glm::vec3 & color);
		static const Texture::CPtr CheckerTexture(const glm::vec3 & color0, const glm::vec3 & color1, float scale = 10.0f);
		static const Texture::CPtr NoiseTexture(size_t mode = 0, const glm::vec3 & color = glm::vec3(1), float scale = 10.0f);
	protected:
		const std::function<glm::vec3(float u, float v, const glm::vec3 & p)> op;
	};
}

#endif // !_OP_TEXTURE_H_
