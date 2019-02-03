#ifndef _ENGINE_LIGHT_H_
#define _ENGINE_LIGHT_H_

#include <CppUtil/Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class LightBase;

		class Light : public Component {
			COMPONENT_SETUP(Light)
		public:
			Light(Basic::Ptr<SObj> sobj, Basic::Ptr<LightBase> light)
				: Component(sobj), light(light) { }

			Basic::Ptr<LightBase> GetLight() const { return light; }
		private:
			Basic::Ptr<LightBase> light;
		};
	}
}

#endif//!_ENGINE_LIGHT_H_
