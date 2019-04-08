#ifndef _ENGINE_COMPONENT_CMPT_LIGHT_H_
#define _ENGINE_COMPONENT_CMPT_LIGHT_H_

#include <CppUtil/Engine/Component.h>

#include <CppUtil/Basic/UGM/Transform.h>

namespace CppUtil {
	namespace Engine {
		class LightBase;

		class CmptLight final : public Component<CmptLight> {
		public:
			CmptLight(Basic::Ptr<SObj> sobj, Basic::Ptr<LightBase> light)
				: Component(sobj), light(light) { }

		public:
			static const Basic::Ptr<CmptLight> New(Basic::Ptr<SObj> sobj, Basic::Ptr<LightBase> light) {
				return Basic::New<CmptLight>(sobj, light);
			}

		protected:
			virtual ~CmptLight() = default;

		public:
			Basic::Transform GetLightToWorldMatrixWithoutScale() const;
		
		public:
			Basic::Ptr<LightBase> light;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_LIGHT_H_
