#ifndef _METAL_H_
#define _METAL_H_

#include <Utility/RTX/Material.h>

namespace RayTracing {
	class Texture;

	class Metal : public Material{
		MATERIAL_SETUP(Metal)
	public:
		Metal(float r, float g, float b, float fuzz = 0.0f);
		Metal(const glm::rgb & specular, float fuzz = 0.0f);
		Metal(CppUtility::Other::CPtr<Texture> specular, float fuzz = 0.0f);

		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(const HitRecord & rec) const;

		const CppUtility::Other::CPtr<Texture> GetTexture() const { return specular; }
		float GetFuzz() const { return fuzz; }
	protected:
		const CppUtility::Other::CPtr<Texture> specular;
		float fuzz;
	};
}

#endif // !_METAL_H_
