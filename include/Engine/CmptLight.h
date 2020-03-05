#ifndef _ENGINE_COMPONENT_CMPT_LIGHT_H_
#define _ENGINE_COMPONENT_CMPT_LIGHT_H_

#include <Engine/Component.h>

#include <UGM/transform.h>

namespace CppUtil {
	namespace Engine {
		class Light;

		class CmptLight final : public Component {
		public:
			CmptLight(Basic::Ptr<SObj> sobj, Basic::Ptr<Light> light)
				: Component(sobj), light(light) { }

		public:
			static const Basic::Ptr<CmptLight> New(Basic::Ptr<SObj> sobj, Basic::Ptr<Light> light) {
				return Basic::New<CmptLight>(sobj, light);
			}

		protected:
			virtual ~CmptLight() = default;

		public:
			Ubpa::transformf GetLightToWorldMatrixWithoutScale() const;
		
		public:
			Basic::Ptr<Light> light;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_LIGHT_H_
