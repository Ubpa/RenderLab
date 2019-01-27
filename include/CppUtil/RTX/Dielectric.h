#ifndef _DIELETRIC_H_
#define _DIELETRIC_H_

#include <CppUtil/RTX/Material.h>

namespace RTX {
	class Dielectric : public Material {
		MATERIAL_SETUP(Dielectric)
	public:
		Dielectric(float refractIndex, glm::vec3 attenuationConst = glm::vec3(0));
		
		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(const HitRecord & rec) const;
		float GetRafractIndex() const { return refractIndex; }
	private:
		float refractIndex;
		glm::vec3 attenuationConst;
	};
}

#endif // !_DIELETRIC_H_
