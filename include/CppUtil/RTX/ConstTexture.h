#ifndef _CONST_TEXTURE_H_
#define _CONST_TEXTURE_H_

#include <CppUtil/RTX/Texture.h>

namespace RTX {
	class ConstTexture : public Texture{
		TEXTURE_SETUP(ConstTexture)
	public:
		ConstTexture(const glm::rgb & color);
		virtual glm::rgb Value(float u = 0, float v = 0, const glm::vec3 & p = glm::vec3(0)) const;
		const glm::rgb & GetColor() const { return color; }
	private:
		glm::rgb color;
	};
}

#endif // !_CONST_TEXTURE_H_
