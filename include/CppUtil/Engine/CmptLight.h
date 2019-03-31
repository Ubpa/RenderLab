#ifndef _ENGINE_COMPONENT_CMPT_LIGHT_H_
#define _ENGINE_COMPONENT_CMPT_LIGHT_H_

#include <CppUtil/Engine/Component.h>

#include <CppUtil/Basic/Transform.h>

namespace CppUtil {
	namespace Engine {
		class Light;

		class CmptLight : public Component {
			COMPONENT_SETUP(CmptLight)
		public:
			CmptLight(Basic::Ptr<SObj> sobj, Basic::Ptr<Light> light)
				: Component(sobj), light(light) { }

			Basic::Ptr<Light> GetLight() const { return light; }
			void SetLight(Basic::Ptr<Light> light) { this->light = light; }
			Basic::Transform GetLightToWorldMatrixWithoutScale() const;
		private:
			Basic::Ptr<Light> light;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_LIGHT_H_
