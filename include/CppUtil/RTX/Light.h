#ifndef _RTX_RT_MATERIAL_LIGHT_H_
#define _RTX_RT_MATERIAL_LIGHT_H_

#include <CppUtil/RTX/Material.h>
#include <glm/vec3.hpp>

namespace RTX {
	class Texture;

	class Light : public Material{
		MATERIAL_SETUP(Light)
	public:
		Light(const glm::vec3 & color, float linear = 0.0f, float quadratic = 0.0f);
		Light(CppUtil::Basic::CPtr<Texture> lightTex, float linear = 0.0f, float quadratic = 0.0f);

		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(const HitRecord & rec) const;
		float GetLinear() const { return linear; }
		float GetQuadratic() const { return quadratic; }
		const CppUtil::Basic::CPtr<Texture> GetTexture() const { return lightTex; }
	private:
		float linear;
		float quadratic;
		CppUtil::Basic::CPtr<Texture> lightTex;
	};
}

#endif // !_RTX_RT_MATERIAL_LIGHT_H_
