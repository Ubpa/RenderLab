#ifndef _ENGINE_COMPONENT_LIGHT_H_
#define _ENGINE_COMPONENT_LIGHT_H_

#include <CppUtil/Engine/Component.h>

#include <glm/mat4x4.hpp>

namespace CppUtil {
	namespace Engine {
		class LightBase;

		class Light : public Component {
			COMPONENT_SETUP(Light)
		public:
			Light(Basic::Ptr<SObj> sobj, Basic::Ptr<LightBase> light)
				: Component(sobj), light(light) { }

			Basic::Ptr<LightBase> GetLight() const { return light; }
			void SetLight(Basic::Ptr<LightBase> light) { this->light = light; }
			glm::mat4 GetLightToWorldMatrixWithoutScale() const;
		private:
			Basic::Ptr<LightBase> light;
		};
	}
}

#endif//!_ENGINE_COMPONENT_LIGHT_H_
