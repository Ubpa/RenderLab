#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <Utility/RTX/Material.h>

namespace RayTracing {
	class Texture;

	class Light : public Material{
		MATERIAL_SETUP(Light)
	public:
		Light(const glm::rgb & color, float linear = 0.0f, float quadratic = 0.0f);
		Light(CppUtility::Other::CPtr<Texture> lightTex, float linear = 0.0f, float quadratic = 0.0f);

		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(const HitRecord & rec) const;
		float GetLinear() const { return linear; }
		float GetQuadratic() const { return quadratic; }
		const CppUtility::Other::CPtr<Texture> GetTexture() const { return lightTex; }
	private:
		float linear;
		float quadratic;
		CppUtility::Other::CPtr<Texture> lightTex;
	};
}

#endif // !_LIGHT_H_
